#include "qkpch.h"
#include "VulkanGraphicsAPI.h"

#include "VulkanBuffer.h"
#include "VulkanGraphicsContext.h"

#include <vulkan/vulkan.hpp>

namespace Quark {

	VulkanGraphicsAPI::~VulkanGraphicsAPI()
	{
		QK_PROFILE_FUNCTION();
	}

	void VulkanGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();
	}

	Scope<GraphicsContext> VulkanGraphicsAPI::CreateGraphicsContext(void* windowHandle)
	{
		return CreateScope<VulkanContext>(windowHandle);
	}

	Ref<VertexBuffer> VulkanGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return CreateRef<VulkanVertexBuffer>(vertices, size);
	}

	Ref<VertexBuffer> VulkanGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return CreateRef<VulkanVertexBuffer>(size);
	}
}
