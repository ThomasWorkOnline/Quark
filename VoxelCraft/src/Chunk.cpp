#include "Chunk.h"

#include "ChunkRenderer.h"
#include "World.h"

static constexpr glm::vec3 s_VertexPositions[24] = {
	// front
	{  0.0f,  0.0f,  1.0f },
	{  1.0f,  0.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  0.0f,  1.0f,  1.0f },

	// right
	{  1.0f,  0.0f,  1.0f },
	{  1.0f,  0.0f,  0.0f },
	{  1.0f,  1.0f,  0.0f },
	{  1.0f,  1.0f,  1.0f },

	// back
	{  1.0f,  0.0f,  0.0f },
	{  0.0f,  0.0f,  0.0f },
	{  0.0f,  1.0f,  0.0f },
	{  1.0f,  1.0f,  0.0f },

	// left
	{  0.0f,  0.0f,  0.0f },
	{  0.0f,  0.0f,  1.0f },
	{  0.0f,  1.0f,  1.0f },
	{  0.0f,  1.0f,  0.0f },

	// top
	{  0.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  0.0f },
	{  0.0f,  1.0f,  0.0f },

	// bottom
	{  1.0f,  0.0f,  1.0f },
	{  0.0f,  0.0f,  1.0f },
	{  0.0f,  0.0f,  0.0f },
	{  1.0f,  0.0f,  0.0f }
};

static ChunkSpecification s_Spec;

const ChunkSpecification& Chunk::GetSpecification()
{
	return s_Spec;
}

static int32_t IndexAtPosition(const glm::ivec3& position)
{
	if (position.x < 0 || position.y < 0 || position.z < 0
		|| position.x >= s_Spec.Width || position.y >= s_Spec.Height || position.z >= s_Spec.Depth)
		return -1;
	return position.x + s_Spec.Width * position.z + s_Spec.Width * s_Spec.Depth * position.y;
}



Chunk::Chunk(const glm::ivec2& position, World* world)
	: m_Coord(position), m_World(world) {}

Chunk::~Chunk()
{
	delete[] m_Blocks;
}

void Chunk::GenerateWorld()
{
	QK_ASSERT(m_Blocks == nullptr, "Tried to allocate chunk more than once");

	m_Blocks = new BlockId[s_Spec.BlockCount];

	for (uint32_t y = 0; y < s_Spec.Height; y++)
	{
		for (uint32_t z = 0; z < s_Spec.Depth; z++)
		{
			for (uint32_t x = 0; x < s_Spec.Width; x++)
			{
				if (!Quark::Application::Get().IsRunning())
					return;

				glm::ivec3 pos = GetBlockPosition({ x, y, z });

				float noise = rand() / static_cast<float>(RAND_MAX);

				uint32_t genBedrock = static_cast<uint32_t>(1.0f + noise * 4.0f);

				BlockId type;
				if (y < genBedrock)
					type = BlockId::Bedrock;
				else if (y >= genBedrock && y < 58)
					type = BlockId::Stone;
				else if (y >= 58 && y < 63)
					type = BlockId::Dirt;
				else if (y >= 63 && y < 64)
					type = BlockId::GrassBlock;
				else
					type = BlockId::Air;

				m_Blocks[IndexAtPosition({ x, y, z })] = { type };
			}
		}
	}
}

void Chunk::GenerateMesh(Chunk* right, Chunk* left, Chunk* front, Chunk* back)
{
	QK_ASSERT(right && left && front && back, "Cannot generate chunk mesh without neighbor information");

	m_VertexCount = 0;
	m_IndexCount = 0;
	m_Vertices.clear();
	m_Indices.clear();

	for (uint32_t y = 0; y < s_Spec.Height; y++)
	{
		for (uint32_t z = 0; z < s_Spec.Depth; z++)
		{
			for (uint32_t x = 0; x < s_Spec.Width; x++)
			{
				if (!Quark::Application::Get().IsRunning())
					return;

				BlockId block = GetBlock({ x, y, z });

				if (block == BlockId::Air)
					continue;

				for (uint8_t f = 0; f < 6; f++)
				{
					if (!IsBlockFaceVisible({ x, y, z }, (BlockFace)f, right, left, front, back))
						continue;

					GenerateFaceVertices({ x, y, z}, block, (BlockFace)f);
				}
			}
		}
	}

	for (uint32_t i = 0; i < m_IndexCount; i += 6)
	{
		GenerateFaceIndices();
	}
}

void Chunk::PushData()
{
	m_Pushed.store(true);

	m_VertexArray = Quark::VertexArray::Create();

	m_VertexBuffer = Quark::VertexBuffer::Create((float*)m_Vertices.data(), m_Vertices.size() * sizeof(BlockVertex));
	m_VertexBuffer->SetLayout({
		{ Quark::ShaderDataType::Float3, "a_Position"  },
		{ Quark::ShaderDataType::Float2, "a_TexCoord"  },
		{ Quark::ShaderDataType::Float,  "a_TexIndex"  },
		{ Quark::ShaderDataType::Float,  "a_Intensity" }
		});
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);

	m_IndexBuffer = Quark::IndexBuffer::Create(m_Indices.data(), m_Indices.size());
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);
}

void Chunk::GenerateFaceVertices(const glm::ivec3& position, BlockId type, BlockFace face)
{
	auto& blockProperties = ChunkRenderer::GetBlockProperties().at(type);

	for (uint8_t i = 0; i < 4; i++)
	{
		BlockVertex v;
		v.Position = s_VertexPositions[i + static_cast<uint8_t>(face) * 4]
			+ glm::vec3(position.x + m_Coord.x * (float)s_Spec.Width, position.y, position.z + m_Coord.y * (float)s_Spec.Depth);
		v.TexCoord = blockProperties.Faces[static_cast<uint8_t>(face)].GetCoords()[i];
		v.TexIndex = 0.0f;
		v.Intensity = (static_cast<uint8_t>(face) + 1) / 6.0f;

		m_Vertices.push_back(v);
	}

	m_IndexCount += 24;
}

void Chunk::GenerateFaceIndices()
{
	m_Indices.push_back(0 + m_VertexCount);
	m_Indices.push_back(1 + m_VertexCount);
	m_Indices.push_back(3 + m_VertexCount);
	m_Indices.push_back(1 + m_VertexCount);
	m_Indices.push_back(2 + m_VertexCount);
	m_Indices.push_back(3 + m_VertexCount);

	m_VertexCount += 4;
}

BlockId Chunk::GetBlock(const glm::ivec3& position) const
{
	int32_t index = IndexAtPosition(position);
	if (index != -1)
	{
		return m_Blocks[index];
	}
	return BlockId::None;
}

glm::ivec3 Chunk::GetBlockPosition(const glm::ivec3& position) const
{
	return { position.x + m_Coord.x * s_Spec.Width, position.y, position.z + m_Coord.y * s_Spec.Depth };
}

void Chunk::ReplaceBlock(const glm::ivec3& position, BlockId type)
{
	int32_t index = IndexAtPosition(position);
	if (index != - 1)
	{
		m_Blocks[index] = { type };

		m_World->OnChunkModified(this);

		if (position.x == 0)
		{
			Chunk* chunk = m_World->GetChunk(m_Coord + glm::ivec2(-1, 0));
			if (chunk)
			{
				m_World->OnChunkModified(chunk);
			}
		}

		if (position.x == s_Spec.Width - 1)
		{
			Chunk* chunk = m_World->GetChunk(m_Coord + glm::ivec2(1, 0));
			if (chunk)
			{
				m_World->OnChunkModified(chunk);
			}
		}

		if (position.z == 0)
		{
			Chunk* chunk = m_World->GetChunk(m_Coord + glm::ivec2(0, -1));
			if (chunk)
			{
				m_World->OnChunkModified(chunk);
			}
		}

		if (position.z == s_Spec.Depth - 1)
		{
			Chunk* chunk = m_World->GetChunk(m_Coord + glm::ivec2(0, 1));
			if (chunk)
			{
				m_World->OnChunkModified(chunk);
			}
		}
	}
}

bool Chunk::IsBlockFaceVisible(const glm::ivec3& position, BlockFace face, Chunk* rightChunk, Chunk* leftChunk, Chunk* frontChunk, Chunk* backChunk) const
{
	glm::ivec3 neighborPosition = position + GetFaceNormal(face);

	bool visible = !IsBlockOpaque(neighborPosition);

	if (neighborPosition.x > s_Spec.Width - 1)
	{
		if (rightChunk)
		{
			if (face == BlockFace::Right)
			{
				if (rightChunk->IsBlockOpaque({ 0, position.y, position.z }) && position.x == s_Spec.Width - 1)
					visible = false;
			}
		}
	}

	if (neighborPosition.x < 0)
	{
		if (leftChunk)
		{
			if (face == BlockFace::Left)
			{
				if (leftChunk->IsBlockOpaque({ s_Spec.Width - 1, position.y, position.z }) && position.x == 0)
					visible = false;
			}
		}
	}

	if (neighborPosition.z > s_Spec.Depth - 1)
	{
		if (frontChunk)
		{
			if (face == BlockFace::Front)
			{
				if (frontChunk->IsBlockOpaque({ position.x, position.y, 0 }) && position.z == s_Spec.Depth - 1)
					visible = false;
			}
		}
	}

	if (neighborPosition.z < 0)
	{
		if (backChunk)
		{
			if (face == BlockFace::Back)
			{
				if (backChunk->IsBlockOpaque({ position.x, position.y, s_Spec.Depth - 1 }) && position.z == 0)
					visible = false;
			}
		}
	}

	return visible;
}

bool Chunk::IsBlockOpaque(const glm::ivec3& position) const
{
	int32_t index = IndexAtPosition(position);
	if (index != -1)
	{
		auto& blockProperties = ChunkRenderer::GetBlockProperties();
		return !blockProperties.at(m_Blocks[index]).Transparent;
	}
	return false;
}
