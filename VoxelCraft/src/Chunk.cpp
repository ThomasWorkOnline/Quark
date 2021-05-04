#include "Chunk.h"

#include "ChunkRenderer.h"
#include "World.h"

static ChunkSpecification s_Spec;

const ChunkSpecification& Chunk::GetSpecification()
{
	return s_Spec;
}

constexpr static int32_t IndexAtPosition(const glm::ivec3& position)
{
	if (position.x < 0 || position.y < 0 || position.z < 0
		|| position.x >= s_Spec.Width || position.y >= s_Spec.Height || position.z >= s_Spec.Depth)
		return -1;
	return position.x + s_Spec.Width * position.z + s_Spec.Width * s_Spec.Depth * position.y;
}

constexpr static glm::ivec3 GetFaceNormal(BlockFace facing)
{
	switch (facing)
	{
	case BlockFace::Front:
		return { 0, 0, 1 };
		break;
	case BlockFace::Right:
		return { 1, 0, 0 };
		break;
	case BlockFace::Back:
		return { 0, 0, -1 };
		break;
	case BlockFace::Left:
		return { -1, 0, 0 };
		break;
	case BlockFace::Top:
		return { 0, 1, 0 };
		break;
	case BlockFace::Bottom:
		return { 0, -1, 0 };
		break;
	default:
		break;
	}
}



Chunk::Chunk(const glm::ivec2& position, World& world)
	: m_Position(position), m_World(world) {}

Chunk::~Chunk()
{
	delete[] m_Blocks;
}



void Chunk::GenerateTerrain()
{
	m_Blocks = new BlockId[s_Spec.BlockCount];

	for (uint32_t y = 0; y < s_Spec.Height; y++)
	{
		for (uint32_t z = 0; z < s_Spec.Depth; z++)
		{
			for (uint32_t x = 0; x < s_Spec.Width; x++)
			{
				if (!Quark::Application::Get().IsRunning())
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
}

void Chunk::GenerateMesh()
{
	m_VertexCount = 0;
	m_IndexCount = 0;
	m_Vertices.clear();
	m_Indices.clear();

	Chunk* rightChunk	= m_World.GetChunk(m_Position + glm::ivec2( 1,  0));
	Chunk* leftChunk	= m_World.GetChunk(m_Position + glm::ivec2(-1,  0));
	Chunk* frontChunk	= m_World.GetChunk(m_Position + glm::ivec2( 0,  1));
	Chunk* backChunk	= m_World.GetChunk(m_Position + glm::ivec2( 0, -1));

	for (uint32_t y = 0; y < s_Spec.Height; y++)
	{
		for (uint32_t z = 0; z < s_Spec.Depth; z++)
		{
			for (uint32_t x = 0; x < s_Spec.Width; x++)
			{
				if (!Quark::Application::Get().IsRunning())
					return;

				BlockId block = GetBlockAt({ x, y, z });

				if (block == BlockId::Air)
					continue;

				for (uint8_t f = 0; f < 6; f++)
				{
					if (!IsBlockFaceVisible({ x, y, z }, (BlockFace)f, rightChunk, leftChunk, frontChunk, backChunk))
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

	m_MeshCreated = true;
	m_UpdatePending = true;
}

bool Chunk::PushData()
{
	bool pushed = m_UpdatePending;
	if (m_UpdatePending)
	{
		// TODO: fix
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

		m_UpdatePending = false;
	}

	return pushed;
}

void Chunk::GenerateFaceVertices(const glm::ivec3& position, BlockId type, BlockFace face)
{
	auto& blockProperties = ChunkRenderer::GetBlockProperties().at(type);

	for (uint8_t i = 0; i < 4; i++)
	{
		BlockVertex v;
		v.Position = s_Spec.VertexPositions[i + static_cast<uint8_t>(face) * 4]
			+ glm::vec3(position.x + m_Position.x * (float)s_Spec.Width, position.y, position.z + m_Position.y * (float)s_Spec.Depth);
		v.TexCoord = blockProperties.Face.GetCoords()[i];
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



bool Chunk::IsBlockFaceVisible(const glm::ivec3& position, BlockFace face, Chunk* rightChunk, Chunk* leftChunk, Chunk* frontChunk, Chunk* backChunk) const
{
	glm::ivec3 neighborPosition = position + GetFaceNormal(face);

	bool visible = !IsBlockOpaqueAt(neighborPosition);

	if (neighborPosition.x > s_Spec.Width - 1)
	{
		if (rightChunk)
		{
			if (face == BlockFace::Right)
			{
				if (rightChunk->IsBlockOpaqueAt({ 0, position.y, position.z }) && position.x == s_Spec.Width - 1)
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
				if (leftChunk->IsBlockOpaqueAt({ s_Spec.Width - 1, position.y, position.z }) && position.x == 0)
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
				if (frontChunk->IsBlockOpaqueAt({ position.x, position.y, 0 }) && position.z == s_Spec.Depth - 1)
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
				if (backChunk->IsBlockOpaqueAt({ position.x, position.y, s_Spec.Depth - 1 }) && position.z == 0)
					visible = false;
			}
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

// TODO: fix chunk regeneration
void Chunk::ReplaceBlock(const glm::ivec3& position, BlockId type)
{
	/*int32_t index = IndexAtPosition(position);
	if (index != - 1)
	{
		m_Blocks[index] = { type };

		// TODO: optimize
		GenerateMesh(true);

		if (position.x == 0)
		{
			Chunk* chunk = m_World.GetChunk(m_Position + glm::ivec2(-1, 0));
			if (chunk)
			{
				chunk->GenerateMesh(true);
			}
		}

		if (position.x == s_Spec.Width - 1)
		{
			Chunk* chunk = m_World.GetChunk(m_Position + glm::ivec2(1, 0));
			if (chunk)
			{
				chunk->GenerateMesh(true);
			}
		}

		if (position.z == 0)
		{
			Chunk* chunk = m_World.GetChunk(m_Position + glm::ivec2(0, -1));
			if (chunk)
			{
				chunk->GenerateMesh(true);
			}
		}

		if (position.z == s_Spec.Depth - 1)
		{
			Chunk* chunk = m_World.GetChunk(m_Position + glm::ivec2(0, 1));
			if (chunk)
			{
				chunk->GenerateMesh(true);
			}
		}
	}*/
}

glm::ivec3 Chunk::GetBlockPositionAbsolute(const glm::ivec3& position) const
{
	return { position.x + m_Position.x * s_Spec.Width, position.y, position.z + m_Position.y * s_Spec.Depth };
}



static std::mutex s_ChunkGenMutex;
void Chunk::GenerateChunk(glm::ivec2 coord, World* world)
{
	std::lock_guard<std::mutex> lock(s_ChunkGenMutex);

	// If a generate request occurs again, ignore it
	if (!world->GetChunk(coord))
	{
		Chunk* newChunk = new Chunk(coord, *world);
		newChunk->GenerateTerrain();

		world->AddChunk(newChunk);
	}
}

static std::future<void> s_ChunkConstructFuture;
static std::future<void> s_ChunkFriendLeftConstructFuture;
static std::future<void> s_ChunkFriendTopConstructFuture;
static std::future<void> s_ChunkFriendRightConstructFuture;
static std::future<void> s_ChunkFriendBottomConstructFuture;

Chunk* Chunk::Load(glm::ivec2 coord, World* world)
{
	QK_TIME_SCOPE_DEBUG(Chunk::Load);

	// First pass
	/// Summary
	/// Generates a cross pattern of chunks around the chunk to be loaded
	/// This step is mendatory to compute the visible faces on chunk edges
	s_ChunkConstructFuture				= std::async(std::launch::async, GenerateChunk, coord, world);
	s_ChunkFriendLeftConstructFuture	= std::async(std::launch::async, GenerateChunk, coord + glm::ivec2(-1,  0), world);
	s_ChunkFriendTopConstructFuture		= std::async(std::launch::async, GenerateChunk, coord + glm::ivec2( 0,  1), world);
	s_ChunkFriendRightConstructFuture	= std::async(std::launch::async, GenerateChunk, coord + glm::ivec2( 1,  0), world);
	s_ChunkFriendBottomConstructFuture	= std::async(std::launch::async, GenerateChunk, coord + glm::ivec2( 0, -1), world);
	s_ChunkConstructFuture.				get();
	s_ChunkFriendLeftConstructFuture.	get();
	s_ChunkFriendTopConstructFuture.	get();
	s_ChunkFriendRightConstructFuture.	get();
	s_ChunkFriendBottomConstructFuture.	get();

	auto newChunk = world->GetChunk(coord);

	// Second pass
	newChunk->GenerateMesh();

	return newChunk;
}
