#include "Environment.h"

namespace Entropy {

	static float s_Vertices[] = {
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};

	static uint32_t s_Indices[] = {
		// front
		0, 2, 3,
		1, 2, 0,

		// right
		4, 6, 7,
		5, 6, 4,

		// back
		8, 10, 11,
		9, 10, 8,

		// left
		12, 14, 15,
		13, 14, 12,

		// bottom
		16, 18, 19,
		17, 18, 16,

		// top
		20, 22, 23,
		21, 22, 20
	};

	Environment::Environment(const std::array<std::string, 6>& filepaths)
	{
		m_CubeMap = CubeMap::Create(filepaths);

		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(s_Vertices, sizeof(s_Vertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" }
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(s_Indices, sizeof(s_Indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	Environment::Environment(const std::string& directory)
	{
		m_CubeMap = Entropy::CubeMap::Create({
		directory + "/posx.jpg",
		directory + "/negx.jpg",
		directory + "/posy.jpg",
		directory + "/negy.jpg",
		directory + "/posz.jpg",
		directory + "/negz.jpg"
			});

		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(s_Vertices, sizeof(s_Vertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" }
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(s_Indices, sizeof(s_Indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Environment::SetEnvironment(const std::string& directory)
	{
		m_CubeMap = Entropy::CubeMap::Create({
			directory + "/posx.jpg",
			directory + "/negx.jpg",
			directory + "/posy.jpg",
			directory + "/negy.jpg",
			directory + "/posz.jpg",
			directory + "/negz.jpg"
		});
	}
}
