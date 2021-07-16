#include "BlockProperties.h"

#include "../Game/Resources.h"

namespace VoxelCraft {

	BlockProperties BlockProperties::Air()
	{
		return {
			{},
			true,
			false,
			nullptr,
			MeshModel::Block,
			{}
		};
	}

	BlockProperties BlockProperties::CreateBlock(const Quark::SubTexture2D& texture, bool transparent, const char* breakSound)
	{
		return {
			{ texture, texture, texture, texture, texture, texture },
			transparent,
			true,
			breakSound,
			MeshModel::Block,
			Resources::GetMeshHitbox(MeshModel::Block)
		};
	}

	BlockProperties BlockProperties::CreateBlock(const Quark::SubTexture2D& top, const Quark::SubTexture2D& bottom, const Quark::SubTexture2D& sides, bool transparent, const char* breakSound)
	{
		return {
			{ sides, sides, sides, sides, top, bottom },
			transparent,
			true,
			breakSound,
			MeshModel::Block,
			Resources::GetMeshHitbox(MeshModel::Block)
		};
	}

	BlockProperties BlockProperties::CreateSprite(const Quark::SubTexture2D& texture, const char* breakSound)
	{
		return {
			{ texture },
			true,
			false,
			breakSound,
			MeshModel::CrossSprite,
			Resources::GetMeshHitbox(MeshModel::CrossSprite)
		};
	}
}
