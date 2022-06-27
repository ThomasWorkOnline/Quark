#include "VulkanApp.h"

VulkanApp::VulkanApp()
{
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
	m_Pipeline.Submit();
}

bool VulkanApp::OnWindowResized(WindowResizedEvent& e)
{
	m_Pipeline.OnViewportResized(e.GetWidth(), e.GetHeight());
	return false;
}
