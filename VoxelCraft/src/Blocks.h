#pragma once

enum class BlockFace : uint8_t
{
	Front = 0,
	Right,
	Back,
	Left,
	Top,
	Bottom
};

enum class BlockId : uint8_t
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
	Quark::SubTexture2D Face;
	const char* BreakSound;
};

struct BlockVertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	float TexIndex;
	float Intensity;
};
