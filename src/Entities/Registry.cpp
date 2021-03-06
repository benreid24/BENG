#include <BLIB/Entities.hpp>

namespace bl
{
namespace entity
{
Registry::Registry()
: dispatcher(nullptr) {}

void Registry::setEventDispatcher(bl::event::Dispatcher& d) { dispatcher = &d; }

Entity Registry::createEntity() {
    const Entity e = IdGenerator::makeNew();
    {
        std::unique_lock lock(entityMutex);
        entities.insert(e);
    }
    if (dispatcher) dispatcher->dispatch<event::EntityCreated>({e});
    return e;
}

bool Registry::createEntity(Entity e) {
    bool created = false;
    {
        std::unique_lock lock(entityMutex);
        if (entities.find(e) == entities.end()) {
            entities.insert(e);
            created = true;
        }
    }

    if (dispatcher && created) dispatcher->dispatch<event::EntityCreated>({e});
    return created;
}

bool Registry::entityExists(Entity e) const { return entities.find(e) != entities.end(); }

void Registry::destroyEntity(Entity e) {
    std::unique_lock lock(entityMutex);
    toDestroy.push_back(e);
}

void Registry::doDestroy() {
    if (dispatcher) {
        for (const Entity e : toDestroy) { dispatcher->dispatch<event::EntityDestroyed>({e}); }
    }
    std::unique_lock lock(entityMutex);
    for (const Entity e : toDestroy) { doDestroy(e); }
    toDestroy.clear();
}

void Registry::doDestroy(Entity e) {
    auto it = entityComponentIterators.find(e);
    if (it != entityComponentIterators.end()) {
        for (const auto& component : it->second) {
            // invalidate views with this component type
            invalidateViews(component.first);

            // remove component -> entity relation
            auto cit = componentEntities.find(component.first);
            if (cit != componentEntities.end()) { cit->second.erase(e); }

            // destroy component
            auto pool = componentPools.find(component.first);
            if (pool != componentPools.end()) {
                pool->second.erase(component.second); // erase from storage
            }
        }
        entityComponentIterators.erase(it);
    }
    entities.erase(e);
}

void Registry::clear() {
    if (dispatcher) {
        for (const Entity e : entities) { dispatcher->dispatch<event::EntityDestroyed>({e}); }
    }

    std::unique_lock lock(entityMutex);
    for (const auto& pair : componentPools) { invalidateViews(pair.first); }

    entities.clear();
    componentPools.clear();
    componentEntities.clear();
    entityComponentIterators.clear();
    toDestroy.clear();
}

Registry::EntityIterator Registry::begin() { return entities.begin(); }

Registry::ConstEntityIterator Registry::begin() const { return entities.begin(); }

Registry::EntityIterator Registry::end() { return entities.end(); }

Registry::ConstEntityIterator Registry::end() const { return entities.end(); }

void Registry::addView(ViewBase* v) {
    std::unique_lock lock(viewMutex);
    activeViews.insert(v);
}

void Registry::removeView(ViewBase* v) {
    std::unique_lock lock(viewMutex);
    activeViews.erase(v);
}

void Registry::invalidateViews(Component::IdType cid) {
    for (ViewBase* view : activeViews) { view->makeDirty(cid); }
}

Registry::ViewBase::~ViewBase() { registry.removeView(this); }

void Registry::ViewBase::makeDirty(Component::IdType cid) {
    if (componentTypes.find(cid) != componentTypes.end()) { dirty = true; }
}

bool Registry::ViewBase::isDirty() const { return dirty; }

void Registry::ViewBase::makeClean() { dirty = false; }

} // namespace entity
} // namespace bl
