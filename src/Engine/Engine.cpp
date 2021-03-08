#include <BLIB/Engine/Engine.hpp>

#include <BLIB/Logging.hpp>
#include <cmath>

namespace bl
{
namespace engine
{
Engine::Engine(const Settings& settings)
: renderWindow(nullptr)
, engineSettings(settings) {}

void Engine::useWindow(sf::RenderWindow& w) { renderWindow = &w; }

event::Dispatcher& Engine::eventBus() { return engineEventBus; }

const Settings& Engine::settings() const { return engineSettings; }

Flags& Engine::flags() { return engineFlags; }

sf::RenderWindow& Engine::window() { return *renderWindow; }

void Engine::nextState(State::Ptr next) { newState = next; }

int Engine::run(State::Ptr initialState) {
    states.push(initialState);

    sf::Clock timer;
    const float minFrameLength =
        engineSettings.maximumFramerate() > 0 ? 1.f / engineSettings.maximumFramerate() : 0.f;
    float lag            = 0.f;
    float lastUpdateTime = timer.getElapsedTime().asSeconds();
    float lastLoopTime   = timer.getElapsedTime().asSeconds();

    sf::Clock updateTimer;
    float updateTimestep    = engineSettings.updateTimestep();
    float averageUpdateTime = engineSettings.updateTimestep();

    float lastWarnTime     = -6.f; // always log first warning
    bool followupLog       = false;
    auto fallBehindWarning = [&lastWarnTime, &updateTimer, &followupLog](float behind) {
        if (updateTimer.getElapsedTime().asSeconds() - lastWarnTime >= 5.f) {
            lastWarnTime = updateTimer.getElapsedTime().asSeconds();
            followupLog  = true;
            BL_LOG_WARN << "Can't catch up, running " << behind << " seconds behind";
        }
    };
    auto skipUpdatesInfo = [&followupLog](int frameCount) {
        if (followupLog) {
            followupLog = false;
            BL_LOG_INFO << "Skipping " << frameCount << " updates";
        }
    };

    while (true) {
        // Clear flags from last loop
        engineFlags.clear();

        // Process window events
        if (renderWindow) {
            sf::Event event;
            while (renderWindow->pollEvent(event)) {
                engineEventBus.dispatch<sf::Event>(event);

                if (event.type == sf::Event::Closed) {
                    renderWindow->close();
                    return 0;
                }
                else if (event.type == sf::Event::LostFocus) {
                    if (!awaitFocus()) {
                        renderWindow->close();
                        return 0;
                    }
                }
                // more events?
            }
        }

        // Update and render
        const float now = timer.getElapsedTime().asSeconds();
        lag += now - lastUpdateTime;
        lastUpdateTime          = now;
        const float startingLag = lag;
        updateTimer.restart();
        while (lag >= updateTimestep) {
            const float updateStart = updateTimer.getElapsedTime().asSeconds();
            states.top()->update(*this, updateTimestep);
            lag -= updateTimestep;
            averageUpdateTime = 0.8f * averageUpdateTime +
                                0.2f * (updateTimer.getElapsedTime().asSeconds() - updateStart);
            if (updateTimer.getElapsedTime().asSeconds() > startingLag * 1.1f) {
                fallBehindWarning(updateTimer.getElapsedTime().asSeconds() - startingLag);
                if (engineSettings.allowVariableTimestep()) {
                    const float newTs = updateTimestep * 1.05f;
                    BL_LOG_INFO << "Adjusting update timestep from " << updateTimestep << "s to "
                                << newTs << "s";
                    updateTimestep    = newTs;
                    averageUpdateTime = updateTimestep;
                }
                else {
                    skipUpdatesInfo(std::ceil(lag / updateTimestep));
                    lag = 0.f;
                }
            }
        }
        if (averageUpdateTime < updateTimestep * 0.9f &&
            updateTimestep > engineSettings.updateTimestep()) {
            float newTs = (1 - averageUpdateTime / updateTimestep) / 2.f * updateTimestep;
            if (newTs < engineSettings.updateTimestep()) newTs = engineSettings.updateTimestep();
            BL_LOG_INFO << "Performance improved, adjusting timestep from " << updateTimestep
                        << "s to " << newTs << "s";
            updateTimestep = newTs;
        }
        states.top()->render(*this, lag);

        // Process flags
        if (engineFlags.active(Flags::Terminate)) {
            if (renderWindow) renderWindow->close();
            return 1;
        }
        else if (engineFlags.active(Flags::PopState)) {
            states.top()->onPoppedOff(*this);
            states.pop();
            if (states.empty()) {
                if (renderWindow) renderWindow->close();
                return 0;
            }
            states.top()->makeActive(*this);
        }

        // Handle state transition
        if (newState) {
            states.top()->onPushedDown(*this);
            states.push(newState);
            states.top()->makeActive(*this);
            newState = nullptr;
        }

        // Adhere to FPS cap
        if (minFrameLength > 0) {
            const float st = minFrameLength - (timer.getElapsedTime().asSeconds() - lastLoopTime);
            if (st > 0) sf::sleep(sf::seconds(st));
            lastLoopTime = timer.getElapsedTime().asSeconds();
        }
    }
}

bool Engine::awaitFocus() {
    sf::Event event;
    while (renderWindow->waitEvent(event)) {
        if (event.type == sf::Event::Closed) return false;
        if (event.type == sf::Event::GainedFocus) return true;
    }
    return false;
}

} // namespace engine
} // namespace bl