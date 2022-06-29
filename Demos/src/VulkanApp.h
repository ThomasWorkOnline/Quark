#pragma once

#include <Quark.h>

using namespace Quark;

class VulkanApp : public Application
{
public:
	VulkanApp();
	virtual ~VulkanApp() override;

	virtual void OnEvent(Event& e) override;
	virtual void OnUpdate(Timestep elapsedTime) override;
	virtual void OnRender() override;

private:
	bool OnWindowResized(WindowResizedEvent& e);

private:
	SceneCamera m_SceneCamera;
	Mat4f m_CameraView;

	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;

	Scope<RenderPipeline> m_RenderPipeline;
};
