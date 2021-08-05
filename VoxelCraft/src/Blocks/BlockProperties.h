#pragma once

#include <Quark.h>

#include "../Game/Models.h"
#include "../Game/Hitbox.h"

namespace VoxelCraft {

	struct BlockProperties
	{
		std::vector<Quark::SubTexture2D> Faces;
		bool Transparent;
		bool CollisionEnabled;
		const char* BreakSound;
		BlockModel Mesh;
		HitBox Hitbox;

		BlockProperties(std::initializer_list<Quark::SubTexture2D> textures, bool transparent, bool collisionEnabled, const char* breakSound, BlockModel meshModel, const HitBox& hitbox)
			: Faces(textures), Transparent(transparent), CollisionEnabled(collisionEnabled), BreakSound(breakSound), Mesh(meshModel), Hitbox(hitbox) {}

		static BlockProperties Air();
		static BlockProperties CreateBlock(const Quark::SubTexture2D& texture, bool transparent, const char* breakSound);
		static BlockProperties CreateBlock(const Quark::SubTexture2D& top, const Quark::SubTexture2D& bottom, const Quark::SubTexture2D& sides, bool transparent, const char* breakSound);
		static BlockProperties CreateSprite(const Quark::SubTexture2D& texture, const char* breakSound);
	};
}
