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

enum class BlockId
{
	None = -1,

	Air = 0,
	Bedrock,
	Dirt,
	GrassBlock,
	Stone,
	Cobblestone
};

struct BlockProperties
{
	bool Transparent = true;
	Entropy::SubTexture2D Texture;
	const char* BreakSound;
};

struct BlockVertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	float TexIndex;
	float Intensity;
};
