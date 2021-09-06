#pragma once

#include "Quark.h"

using namespace Quark;

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
};

class FontTest : public Application
{
public:
	FontTest()
	{
		auto font = m_Library.Load("arial-regular", "assets/fonts/arial.ttf", 0, 48);
		m_Shader = Shader::Create("assets/shaders/default.glsl");

		uint32_t maxIndices = 6 * m_String.size();
		uint32_t maxVertices = 4 * m_String.size();

		{
			float x = 0.0f;
			float y = 0.0f;
			uint32_t n = 0;
			Vertex* vertices = new Vertex[maxVertices];
			for (uint32_t i = 0; i < m_String.size(); i++)
			{
				auto& ch = font->GetCharacter(m_String[i]);

				constexpr float scale = 0.001f;
				constexpr glm::vec3 color = { 1.0f, 1.0f, 0.0f };

				float xpos = (x + ch.Bearing.x) * scale;
				float ypos = (-y - ch.Bearing.y) * scale;
				float w = ch.Size.x * scale;
				float h = ch.Size.y * scale;
				float tx = ch.TexCoordX;

				x += ch.Advance.x;
				y += ch.Advance.y;

				if (!w || !h)
					continue;

				float width = font->GetAtlasWidth();
				float height = font->GetAtlasHeight();

				vertices[n++] = { { xpos + w,	-ypos,		1.0f }, { tx + ch.Size.x / width,	0.0f } };
				vertices[n++] = { { xpos,		-ypos,		1.0f},	{ tx,						0.0f } };
				vertices[n++] = { { xpos,		-ypos - h,	1.0f }, { tx,						ch.Size.y / height } };
				vertices[n++] = { { xpos + w,	-ypos - h,	1.0f }, { tx + ch.Size.x / width,	ch.Size.y / height } };

				/*vertices[i * 4 + 0].Position = glm::vec3(xpos, ypos + h, 1.0f);
				vertices[i * 4 + 1].Position = glm::vec3(xpos,		ypos,		1.0f);
				vertices[i * 4 + 2].Position = glm::vec3(xpos + w,	ypos,		1.0f);
				vertices[i * 4 + 3].Position = glm::vec3(xpos + w,	ypos + h,	1.0f);

				vertices[i * 4 + 0].TexCoord = glm::vec2(0.0f, 0.0f);
				vertices[i * 4 + 1].TexCoord = glm::vec2(h / font->GetAtlasWidth(), 0.0f);
				vertices[i * 4 + 2].TexCoord = glm::vec2(0.0f, 0.0f);
				vertices[i * 4 + 3].TexCoord = glm::vec2(0.0f, 0.0f);*/

				/*for (uint8_t v = 0; v < 6; v++)
				{
					vertices[v + i * 6].TexIndex = 0;
					vertices[v + i * 6].Color = glm::vec3(1.0f, 1.0f, 0.0f);
				}*/
			}

			m_VertexArray = VertexArray::Create();
			auto vbo = VertexBuffer::Create(vertices, sizeof(Vertex) * maxVertices);
			vbo->SetLayout({
					{ ShaderDataType::Float3, "a_Position" },
					{ ShaderDataType::Float2, "a_TexCoord" }
				});
			m_VertexArray->AddVertexBuffer(vbo);

			delete[] vertices;
		}

		{
			uint32_t* indices = new uint32_t[maxIndices];

			uint32_t offset = 0;
			for (uint32_t i = 0; i < maxIndices; i += 6)
			{
				indices[i + 0] = offset + 0;
				indices[i + 1] = offset + 3;
				indices[i + 2] = offset + 2;
				indices[i + 3] = offset + 0;
				indices[i + 4] = offset + 2;
				indices[i + 5] = offset + 1;

				offset += 4;
			}

			auto ibo = IndexBuffer::Create(indices, maxIndices);
			m_VertexArray->SetIndexBuffer(ibo);

			delete[] indices;
		}

		{
			constexpr uint32_t textureSlots = 32;
			int32_t* samplers = new int32_t[textureSlots];
			for (uint32_t i = 0; i < textureSlots; i++)
				samplers[i] = i;

			m_Shader->SetIntArray("u_Samplers", samplers, textureSlots);

			delete[] samplers;
		}

		RenderCommand::BindTextureIDToSlot(font->GetRendererID(), 0);
	}

	void OnUpdate(float elapsedTime)
	{
		m_Camera.OnUpdate();

		m_Shader->Attach();
		m_Shader->SetMat4("u_Projection", m_Camera.GetProjection());
		m_Shader->SetMat4("u_View", glm::mat4(1.0f));

		m_VertexArray->Attach();
		RenderCommand::DrawIndexed(m_VertexArray);
	}

	void OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(FontTest::OnWindowResized));
	}

	bool OnWindowResized(WindowResizedEvent& e)
	{
		m_Camera.SetAspectRatio((float)e.GetWidth() / e.GetHeight());
		return false;
	}

private:
	FontLibrary m_Library;
	Ref<VertexArray> m_VertexArray;
	Ref<Shader> m_Shader;
	PerspectiveCamera m_Camera = { 16.0f / 9.0f, 70.0f };

	std::string m_String = "ABCDEFG";
};
