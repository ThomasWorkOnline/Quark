#pragma once

#include "Quark.h"

using namespace Quark;

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	uint32_t TexLayer;
	uint32_t TexIndex;
};

class TextureArrayTest : public Application
{
public:
	TextureArrayTest()
	{
		TextureArraySpecification spec;
		spec.DataFormat = TextureDataFormat::SRGBA8;
		spec.Width = 16;
		spec.Height = 16;
		spec.Layers = 2;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::Nearest;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Nearest;

		Image defaultTexture("assets/textures/blocks/default_texture.png", true);
		Image oakLeavesTexture("assets/textures/blocks/oak_leaves.png", true);

		m_TextureArray = Texture2DArray::Create(spec);
		m_TextureArray->SetData(defaultTexture.Data(), defaultTexture.Size(), 0);
		m_TextureArray->SetData(oakLeavesTexture.Data(), oakLeavesTexture.Size(), 1);

		m_Shader = Shader::Create("assets/shaders/textureArray.glsl");

		m_VertexArray = VertexArray::Create();

		constexpr Vertex vertices[] = {
			{ { 0.0f, 0.0f, 1.0f },  { 0.0f, 0.0f }, 0, 0 },
			{ { 1.0f, 0.0f, 1.0f },  { 1.0f, 0.0f }, 0, 0 },
			{ { 1.0f, 1.0f, 1.0f },  { 1.0f, 1.0f }, 1, 0 },
			{ { 0.0f, 1.0f, 1.0f },  { 0.0f, 1.0f }, 0, 0 }
		};

		constexpr uint32_t indices[] = {
			0, 3, 2,
			0, 2, 1
		};

		m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Int,    "a_TexLayer" },
			{ ShaderDataType::Int,    "a_TexIndex" }
		});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void OnUpdate(float elapsedTime)
	{
		m_Camera.OnUpdate();

		Renderer::BeginScene(m_Camera.GetProjection(), m_CameraView);

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Renderer::DrawLine(glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0), color, color);

		Renderer::EndScene();

		m_Shader->Attach();
		m_Shader->SetMat4("u_Model", glm::mat4(1.0f));
		m_Shader->SetMat4("u_View", m_CameraView);
		m_Shader->SetMat4("u_Projection", m_Camera.GetProjection());
		m_Shader->SetInt("u_Sampler", 0);

		m_TextureArray->Attach();
		
		m_VertexArray->Attach();
		RenderCommand::DrawIndexed(m_VertexArray);
	}

	void OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(TextureArrayTest::OnWindowResized));
	}

	bool OnWindowResized(WindowResizedEvent& e)
	{
		m_Camera.SetAspectRatio((float)e.GetWidth() / e.GetHeight());
		return false;
	}

private:
	PerspectiveCamera m_Camera = { 16.0f / 9.0f, 70.0f };
	glm::mat4 m_CameraView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	Ref<Texture2DArray> m_TextureArray;
	Ref<Shader> m_Shader;

	Ref<VertexArray> m_VertexArray;
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;
};
