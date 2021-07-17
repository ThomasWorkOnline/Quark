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
			BlockModel::Block,
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
			BlockModel::Block,
			Resources::GetBlockHitbox(BlockModel::Block)
		};
	}

	BlockProperties BlockProperties::CreateBlock(const Quark::SubTexture2D& top, const Quark::SubTexture2D& bottom, const Quark::SubTexture2D& sides, bool transparent, const char* breakSound)
	{
		return {
			{ sides, sides, sides, sides, top, bottom },
			transparent,
			true,
			breakSound,
			BlockModel::Block,
			Resources::GetBlockHitbox(BlockModel::Block)
		};
	}

	BlockProperties BlockProperties::CreateSprite(const Quark::SubTexture2D& texture, const char* breakSound)
	{
		return {
			{ texture },
			true,
			false,
			breakSound,
			BlockModel::CrossSprite,
			Resources::GetBlockHitbox(BlockModel::CrossSprite)
		};
	}
}
