#include "qkpch.h"
#include "Entity.h"

namespace Quark {

    Entity::Entity(entt::entity entity, entt::registry* registry)
        : m_Entity(entity), m_Registry(registry)
    {
    }
}
