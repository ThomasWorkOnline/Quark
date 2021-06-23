#pragma once

#include <Quark.h>

#include "Collision.h"

#include <optional>

struct AxisBounds
{
	float Min;
	float Max;
};

struct AABounds
{
	AxisBounds X;
	AxisBounds Y;
	AxisBounds Z;
};

class HitBox
{
public:
	HitBox(const AABounds& boundaries)
		: m_Bounds(boundaries) {}

	const AABounds& GetBounds() const { return m_Bounds; }

	std::optional<CollisionData> Collide(const glm::vec3& point) const;

	HitBox Move(const glm::vec3& position) const;

private:
	AABounds m_Bounds;
};
