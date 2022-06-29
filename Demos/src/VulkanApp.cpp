#include "VulkanApp.h"

static constexpr Vertex2D s_Vertices[] = {
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
	m_RenderPipeline = RenderPipeline::Create();
	m_SceneCamera.SetOrthographic(1.0f);

	m_VertexBuffer = VertexBuffer::Create(s_Vertices, sizeof(s_Vertices));
	m_VertexBuffer->SetLayout(g_Vertex2DLayout);

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

void VulkanApp::OnUpdate(Timestep elapsedTime)
{
	m_CameraView = glm::rotate(m_CameraView, elapsedTime * glm::radians(90.0f), Vec3f(0.0f, 0.0f, 1.0f));
}

void VulkanApp::OnRender()
{
	m_RenderPipeline->Begin(m_SceneCamera.GetProjection(), m_CameraView);
	m_RenderPipeline->BeginRenderPass();

	m_VertexBuffer->Attach();
	m_IndexBuffer->Attach();
	RenderCommand::DrawIndexed(nullptr, sizeof(s_Indices) / sizeof(uint32_t));

	m_RenderPipeline->EndRenderPass();
	m_RenderPipeline->End();

	m_RenderPipeline->Submit();
}

bool VulkanApp::OnWindowResized(WindowResizedEvent& e)
{
	m_RenderPipeline->OnViewportResized(e.GetWidth(), e.GetHeight());
	return false;
}
