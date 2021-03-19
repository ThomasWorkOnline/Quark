#include "Chunk.h"

#include "World.h"
#include "ChunkRenderer.h"

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
		return { 0,  0,  1 };
		break;
	case Right:
		return { 1,  0,  0 };
		break;
	case Back:
		return { 0,  0, -1 };
		break;
	case Left:
		return { -1,  0,  0 };
		break;
	case Top:
		return { 0,  1,  0 };
		break;
	case Bottom:
		return { 0, -1,  0 };
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

void Chunk::ReplaceBlock(const glm::ivec3& position, BlockType type)
{
	int32_t index = IndexAtPosition(position);
	if (index > 0)
	{
		m_Blocks[index] = Block(type, type == BlockType::Air);

		// TODO: optimize
		m_IndexCount = 0;
		m_Vertices.clear();
		m_Indices.clear();
		ConstructChunkMesh(true);
	}
}

void Chunk::ConstructChunkData(const std::atomic<bool>& running)
{
	m_Blocks = new Block[s_Spec.BlockCount];

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

				BlockType type;
				if (y <= genBedrock)
					type = BlockType::Bedrock;
				else if (y > genBedrock && y <= 58)
					type = BlockType::Stone;
				else if (y > 58 && y <= 63)
					type = BlockType::Dirt;
				else if (y > 63 && y <= 64)
					type = BlockType::GrassBlock;
				else
					type = BlockType::Air;

				m_Blocks[IndexAtPosition({ x, y, z })] = Block(type, type == BlockType::Air);
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

				auto& block = m_Blocks[IndexAtPosition({ x, y, z })];

				if (block.Id == BlockType::Air)
					continue;

				for (uint8_t f = 0; f < 6; f++)
				{
					if (!IsBlockFaceVisible({ x, y, z }, (BlockFace)f))
						continue;

					for (uint8_t i = 0; i < 4; i++)
					{
						Vertex v;
						v.Position = s_Spec.VertexPositions[i + f * 4]
							+ glm::vec3(x + m_Position.x * (float)s_Spec.Width, y, z + m_Position.y * (float)s_Spec.Depth);

						auto& data = ChunkRenderer::GetData();

						glm::vec2 texCoord = s_Spec.VertexTexCoords[i];
						texCoord.x += ((uint32_t)block.Id - 1) % (data.Texture->GetWidth() / data.SubTextureSize.x);
						texCoord.y += ((uint32_t)block.Id - 1) / (data.Texture->GetWidth() / data.SubTextureSize.x);
						texCoord.x /= data.Texture->GetWidth() / data.SubTextureSize.x;
						texCoord.y /= data.Texture->GetHeight() / data.SubTextureSize.y;

						v.TexCoord = texCoord;
						v.TexIndex = (float)block.Id - 1;
						v.Intensity = (f + 1) / 6.0f;

						m_Vertices.emplace_back(std::move(v));

						m_IndexCount += 6;
					}
				}
			}
		}
	}

	ConstructMeshIndices();

	m_UpdatePending = true;
}

void Chunk::PushData()
{
	if (m_UpdatePending)
	{
		m_VertexArray = Entropy::VertexArray::Create();

		m_VertexBuffer = Entropy::VertexBuffer::Create((float*)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
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

void Chunk::ConstructMeshIndices()
{
	m_Indices.reserve(m_IndexCount);

	size_t offset = 0;
	for (uint32_t i = 0; i < m_IndexCount; i += 6)
	{
		m_Indices.push_back(0 + offset);
		m_Indices.push_back(1 + offset);
		m_Indices.push_back(3 + offset);
		m_Indices.push_back(1 + offset);
		m_Indices.push_back(2 + offset);
		m_Indices.push_back(3 + offset);

		offset += 4;
	}
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
		return !m_Blocks[index].Transparent;
	return false;
}

const Block* Chunk::GetBlockAt(const glm::ivec3& position) const
{
	int32_t index = IndexAtPosition(position);
	if (index != -1)
		return &m_Blocks[index];
	else
		return nullptr;
}

glm::ivec3 Chunk::GetBlockPositionAbsolute(const glm::ivec3& position) const
{
	return { position.x + m_Position.x * s_Spec.Width, position.y, position.z + m_Position.y * s_Spec.Depth };
}
