#include <BLIB/Engine/Engine.hpp>

#include <BLIB/Logging.hpp>
#include <cmath>

namespace bl
{
Engine::Engine(sf::RenderWindow& window, const EngineSettings& settings)
: renderWindow(window)
, engineSettings(settings) {}

EngineEventDispatcher& Engine::engineEventDispatcher() { return engineEventBus; }

WindowEventDispatcher& Engine::windowEventDispatcher() { return windowEventBus; }

const EngineSettings& Engine::settings() const { return engineSettings; }

EngineFlags& Engine::flags() { return engineFlags; }

sf::RenderWindow& Engine::window() { return renderWindow; }

void Engine::nextState(EngineState::Ptr next) { newState = next; }

int Engine::run(EngineState::Ptr initialState) {
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

    float lastWarnTime     = updateTimer.getElapsedTime().asSeconds();
    bool followupLog       = false;
    auto fallBehindWarning = [&lastWarnTime, &updateTimer, &followupLog](float behind) {
        if (updateTimer.getElapsedTime().asSeconds() - lastWarnTime >= 5.f) {
            lastWarnTime = updateTimer.getElapsedTime().asSeconds();
            followupLog  = true;
            BL_LOG_WARN << "Can't catch up, running " << behind << " seconds behind";
        }
    };
    auto adjustTimestepInfo = [&followupLog](float oldTs, float newTs) {
        if (followupLog) {
            followupLog = false;
            BL_LOG_INFO << "Adjusting update timestep from " << oldTs << "s to " << newTs
                        << "s";
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
        sf::Event event;
        while (renderWindow.pollEvent(event)) {
            windowEventBus.dispatch(event);

            if (event.type == sf::Event::Closed) {
                renderWindow.close();
                return 0;
            }
            else if (event.type == sf::Event::LostFocus) {
                if (!awaitFocus()) return 0;
            }
            // more events?
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
            averageUpdateTime =
                0.8f * averageUpdateTime +
                0.2f * (updateTimer.getElapsedTime().asSeconds() - updateStart);
            if (updateTimer.getElapsedTime().asSeconds() > startingLag * 1.1f) {
                fallBehindWarning(startingLag - updateTimer.getElapsedTime().asSeconds());
                if (engineSettings.allowVariableTimestep()) {
                    const float newTs = averageUpdateTime * 1.05f;
                    adjustTimestepInfo(updateTimestep, newTs);
                    updateTimestep = newTs;
                }
                else {
                    skipUpdatesInfo(std::ceil(lag / updateTimestep));
                    lag = 0.f;
                }
            }
        }
        states.top()->render(*this, lag);

        // Process flags
        if (engineFlags.flagSet(EngineFlags::Terminate)) {
            renderWindow.close();
            return 1;
        }
        else if (engineFlags.flagSet(EngineFlags::PopState)) {
            states.top()->onPoppedOff(*this);
            states.pop();
            if (states.empty()) {
                renderWindow.close();
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
            const float st =
                minFrameLength - (timer.getElapsedTime().asSeconds() - lastLoopTime);
            if (st > 0) sf::sleep(sf::seconds(st));
            lastLoopTime = timer.getElapsedTime().asSeconds();
        }
    }
}

} // namespace bl
