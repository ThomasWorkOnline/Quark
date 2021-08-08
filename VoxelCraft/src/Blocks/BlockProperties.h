#pragma once

#include <Quark.h>

#include "../Game/Models.h"
#include "../Game/Hitbox.h"

namespace VoxelCraft {

	struct BlockProperties
	{
		std::string Name;

		std::vector<Quark::SubTexture2D> Faces;
		bool Transparent;
		bool CollisionEnabled;
		const char* BreakSound;
		BlockModel Mesh;
		HitBox Hitbox;

		std::vector<uint32_t> Groups;

		static BlockProperties Air();
		static BlockProperties CreateBlock(const std::string& name, const Quark::SubTexture2D& texture, bool transparent, const char* breakSound, std::initializer_list<uint32_t> groups = {});
		static BlockProperties CreateBlock(const std::string& name, const Quark::SubTexture2D& top, const Quark::SubTexture2D& bottom, const Quark::SubTexture2D& sides, bool transparent, const char* breakSound, std::initializer_list<uint32_t> groups = {});
		static BlockProperties CreateSprite(const std::string& name, const Quark::SubTexture2D& texture, const char* breakSound, std::initializer_list<uint32_t> groups = {});
	};
}
