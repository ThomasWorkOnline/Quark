#pragma once

#include <Quark.h>

#include "Collision.h"

#include <optional>

namespace VoxelCraft {

	struct Range
	{
		double Min;
		double Max;
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
		HitBox() = default; // No hitbox
		HitBox(const AABounds& boundaries)
			: m_Bounds(boundaries) {}

		const AABounds& GetBounds() const { return m_Bounds; }

		std::optional<CollisionData> CollideWith(const Position3D& point) const;
		std::optional<AABBCollisionResult> CollideWith(const HitBox& other) const;

		HitBox MoveTo(const Position3D& position) const;

	private:
		AABounds m_Bounds;
	};
}
