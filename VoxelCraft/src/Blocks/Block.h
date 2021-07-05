#pragma once

#include <Quark.h>

class BlockProperties;

enum class BlockFace
{
	Front = 0,
	Right,
	Back,
	Left,
	Top,
	Bottom
};

glm::ivec3 GetFaceNormal(BlockFace facing);

class Block
{
public:
	enum class ID : uint8_t
	{
		Air = 0,
		Bedrock,
		Dirt,
		GrassBlock,
		Stone,
		Cobblestone,
		Poppy,
		Grass
	};

	Block() = default;
	Block(ID id)
		: m_Id(id) {}

	const BlockProperties& GetProperties() const;

	bool operator==(ID id) const { return m_Id == id; }
	bool operator!=(ID id) const { return m_Id != id; }

	bool operator==(Block other) const { return m_Id == other.m_Id; }
	bool operator!=(Block other) const { return m_Id != other.m_Id; }

	ID GetId() const { return m_Id; }

private:
	ID m_Id;
};
