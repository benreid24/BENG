#ifndef BLIB_ENTITIES_EVENTS_HPP
#define BLIB_ENTITIES_EVENTS_HPP

#include <BLIB/Entities/Entity.hpp>

namespace bl
{
namespace entity
{
/// Collection of events pertaining to entities
namespace event
{
/**
 * @brief Fired after an entity is added to the Registry
 *
 * @ingroup Entities
 *
 */
struct EntityCreated {
    /// The entity that was just created
    const Entity entity;
};

/**
 * @brief Fired right before an entity is removed from the registry
 *
 * @ingroup Entities
 *
 */
struct EntityDestroyed {
    /// The entity that is about to get destroyed
    const Entity entity;
};

/**
 * @brief Fired when a component is added to an entity
 *
 * @tparam T The type of component that was added
 * @ingroup Entities
 */
template<typename T>
struct ComponentAdded {
    /// The entity that got the component
    const Entity entity;

    /// The component that was added
    const T& component;

    /**
     * @brief Construct a new ComponentAdded event
     *
     * @param e The entity the component belongs to
     * @param component The component that was added
     */
    ComponentAdded(Entity e, const T& component)
    : entity(e)
    , component(component) {}
};

/**
 * @brief Fired right before a component is removed from an entity
 *
 * @tparam T The type of component that is about to be removed
 * @ingroup Entities
 */
template<typename T>
struct ComponentRemoved {
    /// The entity the component belongs to
    const Entity entity;

    /// The component that is about to be removed
    const T& component;

    /**
     * @brief Construct a new ComponentRemoved event
     *
     * @param e The entity the component belongs to
     * @param component The component that was removed
     */
    ComponentRemoved(Entity e, const T& component)
    : entity(e)
    , component(component) {}
};

} // namespace event
} // namespace entity
} // namespace bl

#endif
