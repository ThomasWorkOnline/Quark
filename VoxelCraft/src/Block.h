#pragma once

enum BlockFace
{
	Front = 0,
	Right,
	Back,
	Left,
	Top,
	Bottom
};

enum class BlockType
{
	Air = 0,
	Dirt,
	Stone
};

struct Block
{
	BlockType Id = BlockType::Air;
	bool Transparent = true;

	Block() = default;
	Block(BlockType id, bool transparent)
		: Id(id), Transparent(transparent) { }
};
