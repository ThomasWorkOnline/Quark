#pragma once

#include "../../Core/Core.h"

namespace Entropy {

	class Entity
	{
	public:
		Entity(glm::vec3 position = glm::vec3(0.0f), glm::quat orientation = glm::quat(glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f))))
			: m_Position(position), m_Orientation(orientation), m_Velocity(0.0f)
		{
		}

		virtual ~Entity() = default;

	protected:
		glm::vec3 m_Position;
		glm::quat m_Orientation;
		glm::vec3 m_Velocity;

	private:

	};
}
