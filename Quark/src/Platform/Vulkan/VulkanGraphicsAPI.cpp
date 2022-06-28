#include "qkpch.h"
#include "VulkanGraphicsAPI.h"

#include "VulkanBuffer.h"
#include "VulkanGraphicsContext.h"

#include <vulkan/vulkan.hpp>

namespace Quark {

	void VulkanGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();
	}

	void VulkanGraphicsAPI::Draw(uint32_t offset, uint32_t count)
	{
		auto commandBuffer = VulkanContext::GetCurrentDevice().GetCommandBuffer();
		commandBuffer.draw(count, 1, offset, 0);
	}

	void VulkanGraphicsAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		auto commandBuffer = VulkanContext::GetCurrentDevice().GetCommandBuffer();
		commandBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
	}

	Ref<VertexBuffer> VulkanGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return CreateRef<VulkanVertexBuffer>(vertices, size);
	}

	Ref<VertexBuffer> VulkanGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return CreateRef<VulkanVertexBuffer>(size);
	}

	Ref<IndexBuffer> VulkanGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return CreateRef<VulkanIndexBuffer>(indices, count);
	}

	Ref<IndexBuffer> VulkanGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return CreateRef<VulkanIndexBuffer>(count);
	}
}
