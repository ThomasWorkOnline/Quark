#include "Entity.h"

namespace Quark {

    Entity::Entity(entt::entity entity, Scene& scene)
        : m_Entity(entity), m_Scene(&scene)
    {
    }
}
