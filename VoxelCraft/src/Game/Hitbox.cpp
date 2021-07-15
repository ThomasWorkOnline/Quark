#include "Hitbox.h"

namespace VoxelCraft {

	std::optional<CollisionData> HitBox::CollideWith(const glm::vec3& point) const
	{
		// Do AABB collision
		bool xCollides = (point.x >= m_Bounds.X.Min && point.x <= m_Bounds.X.Max);
		bool yCollides = (point.y >= m_Bounds.Y.Min && point.y <= m_Bounds.Y.Max);
		bool zCollides = (point.z >= m_Bounds.Z.Min && point.z <= m_Bounds.Z.Max);

		// Collision occurs
		if (xCollides && yCollides && zCollides)
		{
			CollisionData data{};
			data.Impact = point;
			//data.Side = ...
			return data;
		}
		return {};
	}

	std::optional<AABBCollisionResult> HitBox::CollideWith(const HitBox& other) const
	{
		bool xCollides = m_Bounds.X.Min <= other.m_Bounds.X.Max && m_Bounds.X.Max >= other.m_Bounds.X.Min;
		bool yCollides = m_Bounds.Y.Min <= other.m_Bounds.Y.Max && m_Bounds.Y.Max >= other.m_Bounds.Y.Min;
		bool zCollides = m_Bounds.Z.Min <= other.m_Bounds.Z.Max && m_Bounds.Z.Max >= other.m_Bounds.Z.Min;

		if (xCollides && yCollides && zCollides)
		{
			AABBCollisionResult data{};
			return data;
		}
		return {};
	}

	HitBox HitBox::MoveTo(const glm::vec3& position) const
	{
		return AABounds{
			{ m_Bounds.X.Min + position.x, m_Bounds.X.Max + position.x },
			{ m_Bounds.Y.Min + position.y, m_Bounds.Y.Max + position.y },
			{ m_Bounds.Z.Min + position.z, m_Bounds.Z.Max + position.z }
		};
	}
}
