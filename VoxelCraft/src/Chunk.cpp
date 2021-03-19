#include "Chunk.h"

#include "ChunkRenderer.h"
#include "World.h"

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

static glm::ivec3 GetFaceNormal(BlockFace facing)
{
	switch (facing)
	{
	case Front:
		return { 0, 0, 1 };
		break;
	case Right:
		return { 1, 0, 0 };
		break;
	case Back:
		return { 0, 0, -1 };
		break;
	case Left:
		return { -1, 0, 0 };
		break;
	case Top:
		return { 0, 1, 0 };
		break;
	case Bottom:
		return { 0, -1, 0 };
		break;
	default:
		break;
	}
}

Chunk::Chunk(const glm::ivec2& position, World* world)
	: m_Position(position), m_World(world) { }

Chunk::~Chunk()
{
	delete[] m_Blocks;
}

glm::ivec3 Chunk::GetBlockPositionAbsolute(const glm::ivec3& position) const
{
	return { position.x + m_Position.x * s_Spec.Width, position.y, position.z + m_Position.y * s_Spec.Depth };
}

void Chunk::ConstructChunkData(const std::atomic<bool>& running)
{
	m_Blocks = new BlockId[s_Spec.BlockCount];

	for (uint32_t y = 0; y < s_Spec.Height; y++)
	{
		for (uint32_t z = 0; z < s_Spec.Depth; z++)
		{
			for (uint32_t x = 0; x < s_Spec.Width; x++)
			{
				if (!running)
					return;

				glm::ivec3 pos = GetBlockPositionAbsolute({ x, y, z });

				float noise = rand() / (float)RAND_MAX;

				uint32_t genBedrock = 1 + noise * 4.0f;

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

	m_Initialized = true;
}

void Chunk::ConstructChunkMesh(const std::atomic<bool>& running)
{
	for (uint32_t y = 0; y < s_Spec.Height; y++)
	{
		for (uint32_t z = 0; z < s_Spec.Depth; z++)
		{
			for (uint32_t x = 0; x < s_Spec.Width; x++)
			{
				if (!running)
					return;

				BlockId block = GetBlockAt({ x, y, z });

				if (block == BlockId::Air)
					continue;

				for (uint8_t f = 0; f < 6; f++)
				{
					if (!IsBlockFaceVisible({ x, y, z }, (BlockFace)f))
						continue;

					GenerateFaceVertices({ x, y, z}, block, (BlockFace)f);
				}
			}
		}
	}

	m_Indices.reserve(m_IndexCount);
	for (uint32_t i = 0; i < m_IndexCount; i += 6)
	{
		GenerateFaceIndices();
	}

	m_UpdatePending = true;
}

void Chunk::PushData()
{
	if (m_UpdatePending)
	{
		m_VertexArray = Entropy::VertexArray::Create();

		m_VertexBuffer = Entropy::VertexBuffer::Create((float*)m_Vertices.data(), m_Vertices.size() * sizeof(BlockVertex));
		m_VertexBuffer->SetLayout({
			{ Entropy::ShaderDataType::Float3, "a_Position"  },
			{ Entropy::ShaderDataType::Float2, "a_TexCoord"  },
			{ Entropy::ShaderDataType::Float,  "a_TexIndex"  },
			{ Entropy::ShaderDataType::Float,  "a_Intensity" }
			});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = Entropy::IndexBuffer::Create(m_Indices.data(), m_Indices.size());
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_UpdatePending = false;
	}
}

void Chunk::GenerateFaceVertices(const glm::ivec3& position, BlockId type, BlockFace face)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		auto& blockProperties = ChunkRenderer::GetBlockProperties().at(type);

		BlockVertex v;
		v.Position = s_Spec.VertexPositions[i + face * 4]
			+ glm::vec3(position.x + m_Position.x * (float)s_Spec.Width, position.y, position.z + m_Position.y * (float)s_Spec.Depth);
		v.TexCoord = blockProperties.Texture.GetCoords()[i];
		v.TexIndex = 0.0f;
		v.Intensity = (face + 1) / 6.0f;

		m_Vertices.emplace_back(std::move(v));
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

bool Chunk::IsBlockFaceVisible(const glm::ivec3& position, BlockFace face) const
{
	glm::ivec3 neighborPosition = position + GetFaceNormal(face);

	bool visible = !IsBlockOpaqueAt(neighborPosition);

	const Chunk* rightChunk = m_World->GetChunk(m_Position + glm::ivec2(1, 0));
	if (rightChunk)
	{
		if (face == BlockFace::Right)
		{
			if (rightChunk->IsBlockOpaqueAt({ 0, position.y, position.z }) && position.x == s_Spec.Width - 1)
				visible = false;
		}
	}

	const Chunk* leftChunk = m_World->GetChunk(m_Position + glm::ivec2(-1, 0));
	if (leftChunk)
	{
		if (face == BlockFace::Left)
		{
			if (leftChunk->IsBlockOpaqueAt({ s_Spec.Width - 1, position.y, position.z }) && position.x == 0)
				visible = false;
		}
	}

	const Chunk* frontChunk = m_World->GetChunk(m_Position + glm::ivec2(0, 1));
	if (frontChunk)
	{
		if (face == BlockFace::Front)
		{
			if (frontChunk->IsBlockOpaqueAt({ position.x, position.y, 0 }) && position.z == s_Spec.Depth - 1)
				visible = false;
		}
	}

	const Chunk* backChunk = m_World->GetChunk(m_Position + glm::ivec2(0, -1));
	if (backChunk)
	{
		if (face == BlockFace::Back)
		{
			if (backChunk->IsBlockOpaqueAt({ position.x, position.y, s_Spec.Depth - 1 }) && position.z == 0)
				visible = false;
		}
	}
	
	return visible;
}

bool Chunk::IsBlockOpaqueAt(const glm::ivec3& position) const
{
	int32_t index = IndexAtPosition(position);
	if (index != -1)
	{
		auto& blockProperties = ChunkRenderer::GetBlockProperties();
		return !blockProperties.at(m_Blocks[index]).Transparent;
	}
	return false;
}

BlockId Chunk::GetBlockAt(const glm::ivec3& position) const
{
	int32_t index = IndexAtPosition(position);
	if (index != -1)
		return m_Blocks[index];
	else
		return BlockId::None;
}

bool Chunk::SetBlockAt(const glm::ivec3& position, BlockId block)
{
	int32_t index = IndexAtPosition(position);
	if (index != -1)
	{
		m_Blocks[index] = block;
		return true;
	}
	return false;
}

void Chunk::ReplaceBlock(const glm::ivec3& position, BlockId type)
{
	int32_t index = IndexAtPosition(position);
	if (index > 0)
	{
		m_Blocks[index] = { type };

		// TODO: optimize
		m_IndexCount = 0;
		m_VertexCount = 0;
		m_Vertices.clear();
		m_Indices.clear();
		ConstructChunkMesh(true);
	}
}
