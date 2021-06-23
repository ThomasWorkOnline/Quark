#include "Hitbox.h"

std::optional<CollisionData> HitBox::Collide(const glm::vec3& point) const
{
	// Do AABB collision
	bool xCollide = (point.x >= m_Bounds.X.Min && point.x <= m_Bounds.X.Max);
	bool yCollide = (point.y >= m_Bounds.Y.Min && point.y <= m_Bounds.Y.Max);
	bool zCollide = (point.z >= m_Bounds.Z.Min && point.z <= m_Bounds.Z.Max);

	// Collision occurs
	if (xCollide && yCollide && zCollide)
	{
		CollisionData data;
		data.Impact = point;
		//data.Side = ...
		return data;
	}
	return {};
}

HitBox HitBox::Move(const glm::vec3& position) const
{
	return AABounds{
		{ m_Bounds.X.Min + position.x, m_Bounds.X.Max + position.x },
		{ m_Bounds.Y.Min + position.y, m_Bounds.Y.Max + position.y },
		{ m_Bounds.Z.Min + position.z, m_Bounds.Z.Max + position.z }
	};
}
