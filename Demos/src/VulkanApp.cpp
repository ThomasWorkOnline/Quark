#include "VulkanApp.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"

static constexpr Vertex s_Vertices[] = {
	{ { -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f } },
	{ {  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f } },
	{ {  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f } },
	{ { -0.5f,  0.5f}, { 1.0f,  1.0f,  1.0f } }
};

static constexpr uint32_t s_Indices[] = {
	0, 1, 2,
	2, 3, 0
};

VulkanApp::VulkanApp()
{
	BufferLayout layout = {
		{ ShaderDataType::Float2, "a_Position" },
		{ ShaderDataType::Float3, "a_Color"    }
	};

	m_VertexBuffer = VertexBuffer::Create(s_Vertices, sizeof(s_Vertices));
	m_VertexBuffer->SetLayout(layout);

	m_IndexBuffer = IndexBuffer::Create(s_Indices, sizeof(s_Indices) / sizeof(uint32_t));
}

VulkanApp::~VulkanApp()
{
}

void VulkanApp::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(OnWindowResized));
}

void VulkanApp::OnRender()
{
	m_Pipeline.Begin();
	m_Pipeline.BeginRenderPass();

	m_VertexBuffer->Attach();
	m_IndexBuffer->Attach();

	RenderCommand::DrawIndexed(nullptr, sizeof(s_Indices) / sizeof(uint32_t));

	m_Pipeline.EndRenderPass();
	m_Pipeline.End();

	m_Pipeline.Submit();
}

bool VulkanApp::OnWindowResized(WindowResizedEvent& e)
{
	m_Pipeline.OnViewportResized(e.GetWidth(), e.GetHeight());
	return false;
}
