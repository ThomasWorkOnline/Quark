#pragma once

#include <Quark.h>

#include "MeshModel.h"

struct BlockProperties
{
	std::vector<Quark::SubTexture2D> Faces;
	bool Transparent = true;
	const char* BreakSound;
	MeshModel Mesh;

	BlockProperties(const std::initializer_list<Quark::SubTexture2D>& textures, bool transparent, const char* breakSound, MeshModel meshModel)
		: Faces(textures), Transparent(transparent), BreakSound(breakSound), Mesh(meshModel) {}

	static inline BlockProperties CreateBlock(const Quark::SubTexture2D& texture, bool transparent, const char* breakSound)
	{
		return {
			{ texture, texture, texture, texture, texture, texture },
			transparent,
			breakSound,
			MeshModel::Block
		};
	}

	static inline BlockProperties CreateBlock(const Quark::SubTexture2D& top, const Quark::SubTexture2D& bottom, const Quark::SubTexture2D& sides, bool transparent, const char* breakSound)
	{
		return {
			{ sides, sides, sides, sides, top, bottom },
			transparent,
			breakSound,
			MeshModel::Block
		};
	}

	static inline BlockProperties CreateSprite(const Quark::SubTexture2D& texture, const char* breakSound)
	{
		return {
			{ texture },
			true,
			breakSound,
			MeshModel::CrossSprite
		};
	}
};
