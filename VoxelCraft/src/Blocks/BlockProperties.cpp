#include "BlockProperties.h"

#include "../Game/Resources.h"

namespace VoxelCraft {

	BlockProperties BlockProperties::Air()
	{
		return {
			"voxelcraft:air_block",
			{},
			true,
			false,
			nullptr,
			BlockModel::Block,
			{}
		};
	}

	BlockProperties BlockProperties::CreateBlock(const std::string& name, const Quark::SubTexture2D& texture, bool transparent, const char* breakSound, std::initializer_list<uint32_t> groups)
	{
		return {
			name,
			{ texture, texture, texture, texture, texture, texture },
			transparent,
			true,
			breakSound,
			BlockModel::Block,
			Resources::GetBlockHitbox(BlockModel::Block),
			groups
		};
	}

	BlockProperties BlockProperties::CreateBlock(const std::string& name, const Quark::SubTexture2D& top, const Quark::SubTexture2D& bottom, const Quark::SubTexture2D& sides, bool transparent, const char* breakSound, std::initializer_list<uint32_t> groups)
	{
		return {
			name,
			{ sides, sides, sides, sides, top, bottom },
			transparent,
			true,
			breakSound,
			BlockModel::Block,
			Resources::GetBlockHitbox(BlockModel::Block),
			groups
		};
	}

	BlockProperties BlockProperties::CreateSprite(const std::string& name, const Quark::SubTexture2D& texture, const char* breakSound, std::initializer_list<uint32_t> groups)
	{
		return {
			name,
			{ texture },
			true,
			false,
			breakSound,
			BlockModel::CrossSprite,
			Resources::GetBlockHitbox(BlockModel::CrossSprite),
			groups
		};
	}
}
