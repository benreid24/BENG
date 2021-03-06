#include <BLIB/Logging.hpp>
#include <BLIB/Media/Audio/AudioSystem.hpp>
#include <BLIB/Util/Random.hpp>

#include <chrono>
#include <limits>

namespace bl
{
namespace
{
constexpr unsigned int UpdatePeriod    = 100;  // milliseconds
constexpr float FadeAmount             = 0.1f; // 10hz updates = 1 second fade
constexpr float FadeTolerance          = 0.001f;
constexpr float DefaultMinFadeDistance = 64.f;
constexpr float DefaultAttenuation     = 1.f;
constexpr float DefaultMaxDistance     = 320.f * 320.f;
constexpr float SoundCapacityFactor    = 0.2f; // 20% size:capacity ratio

} // namespace

AudioSystem::AudioSystem()
: masterVolume(100.f)
, state(SystemState::Running)
, runner(&AudioSystem::background, this)
, musicVolumeFactor(1)
, musicState(MusicState::Stopped)
, fadeVolumeFactor(-1.f)
, defaultSpatialSettings{DefaultMinFadeDistance, DefaultAttenuation}
, maxSpatialDistanceSquared(DefaultMaxDistance) {
    sf::Listener::setUpVector(0, 0, -1); // TODO - correct?
    BL_LOG_INFO << "AudioSystem online";
}

AudioSystem::~AudioSystem() {
    stopAll();
    state = SystemState::Stopping;
    pauseSync.notify_all();
    runner.join();
    BL_LOG_INFO << "AudioSystem shutdown";
}

void AudioSystem::setVolume(float v) { masterVolume = v; }

void AudioSystem::stopAll(bool fade) {
    fadeVolumeFactor = (fade && state != SystemState::Paused) ? 1.f : 0.0001f;
}

void AudioSystem::pause() {
    std::unique_lock lock(pauseMutex);
    state = SystemState::Paused;
}

void AudioSystem::resume() {
    if (state == SystemState::Paused) {
        state = SystemState::Running;
        pauseSync.notify_all();
    }
}

void AudioSystem::setListenerPosition(const sf::Vector2f& pos) {
    sf::Listener::setPosition({pos.x, pos.y, 0});
}

void AudioSystem::setDefaultSpatialSoundSettings(const SpatialSettings& settings) {
    std::unique_lock lock(soundMutex);
    defaultSpatialSettings = settings;
}

void AudioSystem::setSpatialSoundCutoffDistance(float md) {
    std::unique_lock lock(soundMutex);
    maxSpatialDistanceSquared = md;
}

void AudioSystem::pushPlaylist(const Playlist& np) {
    std::unique_lock lock(playlistMutex);

    playlists.emplace_back(new Playlist(np));
    playlists.back()->setVolume(volume() * musicVolumeFactor);
    if (playlists.size() == 1) { musicState = MusicState::Playing; }
    else {
        musicState        = MusicState::Pushing;
        musicVolumeFactor = 1.f;
    }
    if (state != SystemState::Paused) playlists.back()->play();
}

void AudioSystem::replacePlaylist(const Playlist& np) {
    std::unique_lock lock(playlistMutex);

    playlists.emplace_back(new Playlist(np));
    playlists.back()->setVolume(volume() * musicVolumeFactor);
    if (playlists.size() == 1) { musicState = MusicState::Playing; }
    else {
        musicState        = MusicState::Replacing;
        musicVolumeFactor = 1.f;
    }
    if (state != SystemState::Paused) playlists.back()->play();
}

void AudioSystem::popPlaylist() {
    if (!playlists.empty()) {
        std::unique_lock lock(playlistMutex);
        musicState = MusicState::Popping;
    }
}

AudioSystem::Handle AudioSystem::create() const {
    return Random::get<Handle>(1, std::numeric_limits<Handle>::max());
}

AudioSystem::Handle AudioSystem::playSound(Resource<sf::SoundBuffer>::Ref sound, bool loop) {
    const Handle h = create();
    std::unique_lock lock(soundMutex);
    auto s = *sounds.emplace(new Sound(h, sound));
    s->sound.setVolume(volume());
    s->sound.setLoop(loop);
    if (state != SystemState::Paused) s->sound.play();
    soundMap.emplace(h, s);
    return h;
}

AudioSystem::Handle AudioSystem::playSpatialSound(Resource<sf::SoundBuffer>::Ref sound,
                                                  const sf::Vector2f& pos, bool loop) {
    return playSpatialSound(sound, pos, defaultSpatialSettings, loop);
}

AudioSystem::Handle AudioSystem::playSpatialSound(Resource<sf::SoundBuffer>::Ref sound,
                                                  const sf::Vector2f& pos,
                                                  const SpatialSettings& settings, bool loop) {
    std::unique_lock lock(soundMutex);
    const float dx = pos.x - sf::Listener::getPosition().x;
    const float dy = pos.y - sf::Listener::getPosition().y;
    if (dx * dx + dy * dy >= maxSpatialDistanceSquared) return InvalidHandle;

    const Handle h = create();
    auto s         = *sounds.emplace(new Sound(h, sound));
    s->sound.setVolume(volume());
    s->sound.setPosition({pos.x, pos.y, 0});
    s->sound.setMinDistance(settings.fadeStartDistance);
    s->sound.setAttenuation(settings.attenuation);
    s->sound.setLoop(loop);
    if (state != SystemState::Paused) s->sound.play();
    soundMap.emplace(h, s);
    return h;
}

sf::Sound* AudioSystem::getSound(Handle h) {
    std::shared_lock lock(soundMutex);
    auto it = soundMap.find(h);
    if (it == soundMap.end()) return nullptr;
    if (!it->second->sound.getLoop()) return nullptr;
    return &it->second->sound;
}

void AudioSystem::stopSound(Handle h) {
    std::unique_lock lock(soundMutex);
    auto it = soundMap.find(h);
    if (it == soundMap.end()) return;
    it->second->sound.stop();
}

float AudioSystem::volume() const {
    if (fadeVolumeFactor < 0) return masterVolume;
    return masterVolume * fadeVolumeFactor;
}

void AudioSystem::background() {
    const auto crossfade = [this]() {
        musicVolumeFactor -= FadeAmount;
        const float f = std::max(0.f, musicVolumeFactor);
        playlists.back()->setVolume(volume() * f);
        playlists.back()->update();
        if (playlists.size() > 1) { // cross fade
            playlists[playlists.size() - 2]->update();
            playlists[playlists.size() - 2]->play();
            playlists[playlists.size() - 2]->setVolume(volume() * (1.f - f));
        }
    };

    const auto finishCrossfade = [this]() {
        musicVolumeFactor = 1.f;
        if (!playlists.empty()) {
            playlists.back()->setVolume(volume());
            playlists.back()->play();
        }
        musicState = playlists.empty() ? MusicState::Stopped : MusicState::Playing;
    };

    while (state != SystemState::Stopping || fadeVolumeFactor > 0.f) {
        {
            // sleep if paused
            std::unique_lock lock(pauseMutex);
            if (state == SystemState::Paused) {
                {
                    std::unique_lock mlock(playlistMutex);
                    std::unique_lock slock(soundMutex);
                    for (auto& s : playlists) { s->pause(); }
                    for (auto& s : sounds) { s->sound.pause(); }
                }
                pauseSync.wait(lock);

                // unpaused
                std::unique_lock mlock(playlistMutex);
                std::unique_lock slock(soundMutex);
                for (auto& s : sounds) {
                    if (s->sound.getStatus() == sf::Sound::Paused) s->sound.play();
                }
                if (playlists.size() > 2) {
                    for (unsigned int i = 0; i < playlists.size(); ++i) { playlists[i]->stop(); }
                }
                if (!playlists.empty()) {
                    playlists.back()->play();
                    if (musicState != MusicState::Playing && playlists.size() > 1) {
                        playlists[playlists.size() - 1]->play(); // resume crossfade
                    }
                }
            }

            // Update fadeout
            if (fadeVolumeFactor > 0.f) {
                if (fadeVolumeFactor <= FadeTolerance) { // done fading out
                    std::unique_lock mlock(playlistMutex);
                    std::unique_lock slock(soundMutex);

                    sounds.clear();
                    soundMap.clear();
                    playlists.clear();
                    musicState       = MusicState::Stopped;
                    fadeVolumeFactor = -1.f;
                }
                // volumes are updated below
                fadeVolumeFactor = std::max(fadeVolumeFactor - FadeAmount, 0.0001f);
            }

            // Update playlists
            {
                std::unique_lock lock(playlistMutex);

                switch (musicState) {
                case MusicState::Popping:
                    crossfade();
                    if (musicVolumeFactor <= 0) { // fade complete
                        playlists.pop_back();
                        finishCrossfade();
                    }
                    break;

                case MusicState::Replacing:
                case MusicState::Pushing:
                    crossfade();
                    if (musicVolumeFactor <= 0) { // fade complete
                        if (musicState != MusicState::Pushing && playlists.size() > 1) { // replace
                            playlists[playlists.size() - 2]->stop();
                            playlists.erase(playlists.begin() + playlists.size() - 2);
                        }
                        finishCrossfade();
                    }
                    break;

                case MusicState::Playing:
                    playlists.back()->update();
                    playlists.back()->setVolume(volume());
                    break;

                default:
                    break;
                }
            }

            // update sounds
            {
                std::unique_lock lock(soundMutex);

                const float v = volume();
                for (auto it = sounds.begin(); it != sounds.end(); ++it) {
                    if (it->get()->sound.getStatus() == sf::Sound::Stopped) {
                        soundMap.erase(it->get()->handle);
                        sounds.erase(it); // only partially invalidated, can still increment
                    }
                    else {
                        it->get()->sound.setVolume(v);
                    }
                }

                const float loadFactor =
                    static_cast<float>(sounds.size()) / static_cast<float>(sounds.capacity());
                if (loadFactor <= SoundCapacityFactor) { sounds.shrink(); }
            }
        }
        // sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(UpdatePeriod));
    }
}

} // namespace bl
