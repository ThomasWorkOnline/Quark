#pragma once

#include <Quark.h>

#include "Collision.h"

#include <optional>

struct Range
{
	float Min;
	float Max;
};

struct AABounds
{
	Range X;
	Range Y;
	Range Z;
};

class HitBox
{
public:
	HitBox(const AABounds& boundaries)
		: m_Bounds(boundaries) {}

	const AABounds& GetBounds() const { return m_Bounds; }

	std::optional<CollisionData> CollideWith(const glm::vec3& point) const;
	std::optional<AABBCollisionResult> CollideWith(const HitBox& other) const;

	HitBox MoveTo(const glm::vec3& position) const;

private:
	AABounds m_Bounds;
};
