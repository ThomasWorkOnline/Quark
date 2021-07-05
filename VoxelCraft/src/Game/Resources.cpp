#include "Resources.h"

std::unordered_map<Block::ID, BlockProperties> Resources::s_BlockProperties;
std::unordered_map<MeshModel, MeshProperties> Resources::s_MeshModels;

Quark::Ref<Quark::VertexArray> Resources::s_CrosshairVertexArray;

Quark::ShaderLibrary Resources::s_ShaderLibrary;

Quark::Ref<Quark::Texture2D> Resources::s_Texture;

const Quark::BufferLayout Resources::s_BufferLayout = {
	{ Quark::ShaderDataType::Float3, "a_Position"  },
	{ Quark::ShaderDataType::Float2, "a_TexCoord"  },
	{ Quark::ShaderDataType::Float,  "a_Intensity" }
};

const Quark::BufferLayout Resources::s_CrosshairBufferLayout = {
	{ Quark::ShaderDataType::Float3, "a_Position"  }
};

static constexpr glm::vec3 s_BlockVertexPositions[24] = {
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
static constexpr glm::vec3 s_CrossSpriteVertexPositions[16] = {
	// first
	{  0.15f,  0.00f,  0.85f },
	{  0.85f,  0.00f,  0.15f },
	{  0.85f,  1.00f,  0.15f },
	{  0.15f,  1.00f,  0.85f },

	// first back
	{  0.85f,  0.00f,  0.15f },
	{  0.15f,  0.00f,  0.85f },
	{  0.15f,  1.00f,  0.85f },
	{  0.85f,  1.00f,  0.15f },

	// second
	{  0.15f,  0.00f,  0.15f },
	{  0.85f,  0.00f,  0.85f },
	{  0.85f,  1.00f,  0.85f },
	{  0.15f,  1.00f,  0.15f },

	// second back
	{  0.85f,  0.00f,  0.85f },
	{  0.15f,  0.00f,  0.15f },
	{  0.15f,  1.00f,  0.15f },
	{  0.85f,  1.00f,  0.85f }
};

static constexpr glm::vec3 s_CrosshairVertexPositions[12] = {
	// horizontal bottom right
	{ -0.010f, -0.001f,  -1.0f },
	{  0.010f, -0.001f,  -1.0f },
	{  0.010f,  0.001f,  -1.0f },
		  
	// horizontal top left
	{  0.010f,  0.001f,  -1.0f },
	{ -0.010f,  0.001f,  -1.0f },
	{ -0.010f, -0.001f,  -1.0f },

	// horizontal bottom right
	{ -0.001f, -0.010f,  -1.0f },
	{  0.001f,  0.010f,  -1.0f },
	{ -0.001f,  0.010f,  -1.0f },

	// horizontal top left
	{  0.001f,  0.010f,  -1.0f },
	{ -0.001f, -0.010f,  -1.0f },
	{  0.001f, -0.010f,  -1.0f }
};
static constexpr uint32_t s_CrosshairIndices[12] = {
	0, 2, 1,
	3, 5, 4,
	6, 8, 7,
	9, 11, 10
};

static constexpr AABounds s_BlockBounds = {
	{ 0.0f, 1.0f },
	{ 0.0f, 1.0f },
	{ 0.0f, 1.0f }
};
static constexpr AABounds s_CrossSpriteBounds = {
	{ 0.15f, 0.85f },
	{ 0.00f, 1.00f },
	{ 0.15f, 0.85f }
};
static constexpr AABounds s_PlayerBounds = {
	{ -0.40f, 0.40f },
	{  0.00f, 1.85f },
	{ -0.40f, 0.40f }
};

const HitBox Resources::s_PlayerHitbox = {
	s_PlayerBounds
};

void Resources::Initialize()
{
	s_ShaderLibrary.Load("default", "assets/shaders/default.glsl");
	s_ShaderLibrary.Load("crosshair", "assets/shaders/crosshair.glsl");
	s_ShaderLibrary.Load("debugMesh", "assets/shaders/debugMesh.glsl");

	s_Texture = Quark::Texture2D::Create("assets/textures/sprite_sheet.png");

	s_BlockProperties = {
		{ Block::ID::Air,			BlockProperties::Air() },
		{ Block::ID::Bedrock,		BlockProperties::CreateBlock({ s_Texture, { 0, 0 }, SubTextureSize }, false, "assets/sounds/break_stone.mp3") },
		{ Block::ID::Dirt,			BlockProperties::CreateBlock({ s_Texture, { 1, 0 }, SubTextureSize }, false, "assets/sounds/break_dirt.mp3") },
		{ Block::ID::GrassBlock,	BlockProperties::CreateBlock({ s_Texture, { 2, 0 }, SubTextureSize }, { s_Texture, { 1, 0 }, SubTextureSize }, { s_Texture, { 2, 1 }, SubTextureSize }, false, "assets/sounds/break_grass.mp3") },
		{ Block::ID::Stone,			BlockProperties::CreateBlock({ s_Texture, { 0, 1 }, SubTextureSize }, false, "assets/sounds/break_stone.mp3") },
		{ Block::ID::Cobblestone,	BlockProperties::CreateBlock({ s_Texture, { 1, 1 }, SubTextureSize }, false, "assets/sounds/break_stone.mp3") },

		{ Block::ID::Poppy,			BlockProperties::CreateSprite({ s_Texture, { 0, 2 }, SubTextureSize }, "assets/sounds/break_grass.mp3") },
		{ Block::ID::Grass,			BlockProperties::CreateSprite({ s_Texture, { 1, 2 }, SubTextureSize }, "assets/sounds/break_grass.mp3") }
	};

	s_MeshModels = {
		{ MeshModel::Block,			MeshProperties::Create(s_BlockVertexPositions, { s_BlockBounds }) },
		{ MeshModel::CrossSprite,	MeshProperties::Create(s_CrossSpriteVertexPositions, { s_CrossSpriteBounds }) }
	};

	s_CrosshairVertexArray = Quark::VertexArray::Create();

	auto vbo = Quark::VertexBuffer::Create((float*)s_CrosshairVertexPositions, sizeof(s_CrosshairVertexPositions));
	vbo->SetLayout(s_CrosshairBufferLayout);
	s_CrosshairVertexArray->AddVertexBuffer(vbo);

	auto ibo = Quark::IndexBuffer::Create((uint32_t*)s_CrosshairIndices, sizeof(s_CrosshairIndices) / sizeof(uint32_t));
	s_CrosshairVertexArray->SetIndexBuffer(ibo);
}
