#pragma once

#include <Quark.h>

#include "../Rendering/MeshModel.h"

struct BlockProperties
{
	std::vector<Quark::SubTexture2D> Faces;
	bool Transparent;
	bool CollisionEnabled;
	const char* BreakSound;
	MeshModel Mesh;

	BlockProperties(const std::initializer_list<Quark::SubTexture2D>& textures, bool transparent, bool collisionEnabled, const char* breakSound, MeshModel meshModel)
		: Faces(textures), Transparent(transparent), CollisionEnabled(collisionEnabled), BreakSound(breakSound), Mesh(meshModel) {}

	static inline BlockProperties Air()
	{
		return {
			{},
			true,
			false,
			nullptr,
			MeshModel::Block
		};
	}

	static inline BlockProperties CreateBlock(const Quark::SubTexture2D& texture, bool transparent, const char* breakSound)
	{
		return {
			{ texture, texture, texture, texture, texture, texture },
			transparent,
			true,
			breakSound,
			MeshModel::Block
		};
	}

	static inline BlockProperties CreateBlock(const Quark::SubTexture2D& top, const Quark::SubTexture2D& bottom, const Quark::SubTexture2D& sides, bool transparent, const char* breakSound)
	{
		return {
			{ sides, sides, sides, sides, top, bottom },
			transparent,
			true,
			breakSound,
			MeshModel::Block
		};
	}

	static inline BlockProperties CreateSprite(const Quark::SubTexture2D& texture, const char* breakSound)
	{
		return {
			{ texture },
			true,
			false,
			breakSound,
			MeshModel::CrossSprite
		};
	}
};
