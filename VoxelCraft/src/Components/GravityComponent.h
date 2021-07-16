#pragma once

namespace VoxelCraft {

	struct GravityComponent
	{
		GravityComponent(float gravity, bool affected = true)
			: GravityConstant(gravity), Affected(affected) {}

		float GravityConstant;
		bool Affected;
	};
}
