#include "Entity.h"

namespace Entropy {

    Entity::Entity(entt::entity entity, Scene* scene)
        : m_Entity(entity), m_Scene(scene)
    {
    }
}
