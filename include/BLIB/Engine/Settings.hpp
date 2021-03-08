#ifndef BLIB_ENGINE_SETTINGS_HPP
#define BLIB_ENGINE_SETTINGS_HPP

namespace bl
{
namespace engine
{
/**
 * @brief Collection of settings to create an Engine with, organized as a builder
 *
 * @ingroup Engine
 *
 */
class Settings {
public:
    static constexpr float DefaultUpdateInterval       = 1.f / 120.f;
    static constexpr float DefaultMaximumFramerate     = 0.f;
    static constexpr bool DefaultAllowVariableTimestep = true;

    /**
     * @brief Creates a new settings object with all default settings
     *
     */
    Settings();

    /**
     * @brief Sets the update interval to use for fixed physics updates
     *
     * @param interval Fixed timestep interval, in seconds
     * @return Settings& Reference to this object
     */
    Settings& withUpdateInterval(float interval);

    /**
     * @brief Sets the maximum framerate in fps. Pass 0 for no cap
     *
     * @param maxFps Maximum framerate in fps
     * @return Settings& Reference to this object
     */
    Settings& withMaxFramerate(float maxFps);

    /**
     * @brief Sets whether or not the engine may vary the update interval to catchup if the
     *        simulation falls behind. If a variable timestep is allowed, the update interval
     *        will be increased to match how long the updates take on average. The update
     *        interval may be adjusted down if performance improves. If a variable timestep is
     *        not allowed then updates will be skipped in order to catchup to real time.
     *
     * @param allow True to allow a variable timestep, false to skip frames to catchup
     * @return Settings& A reference to this object
     */
    Settings& withAllowVariableTimestep(bool allow);

    /**
     * @brief Returns the fixed physics update interval, in seconds
     *
     */
    float updateTimestep() const;

    /**
     * @brief Returns the maximum rendering framerate, in fps
     *
     */
    float maximumFramerate() const;

    /**
     * @brief Returns whether or not a variable timestep may be used
     *
     */
    bool allowVariableTimestep() const;

private:
    float updateTime;
    float maxFps;
    bool allowVariableInterval;
};

} // namespace engine
} // namespace bl

#endif