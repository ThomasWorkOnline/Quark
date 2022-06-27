#pragma once

#include "Platform/Vulkan/VulkanPipeline.h"
#include <Quark.h>

using namespace Quark;

class VulkanApp : public Application
{
public:
	VulkanApp();
	virtual ~VulkanApp() override;

	virtual void OnEvent(Event& e) override;
	virtual void OnRender() override;

private:
	bool OnWindowResized(WindowResizedEvent& e);

private:
	VulkanPipeline m_Pipeline;
};
