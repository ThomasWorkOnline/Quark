#include "qkpch.h"
#include "VulkanGraphicsAPI.h"

#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanCubemap.h"
#include "VulkanFont.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanTextureArray.h"
#include "VulkanUniformBuffer.h"

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

	Ref<Cubemap> VulkanGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return CreateRef<VulkanCubemap>(spec);
	}

	Ref<Font> VulkanGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return CreateRef<VulkanFont>(filepath, fontSize);
	}

	Ref<Framebuffer> VulkanGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return CreateRef<VulkanFramebuffer>(spec);
	}

	Scope<RenderPipeline> VulkanGraphicsAPI::CreateRenderPipeline()
	{
		return CreateScope<VulkanPipeline>();
	}

	Ref<Shader> VulkanGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return CreateRef<VulkanShader>(filepath);
	}

	Ref<Shader> VulkanGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return CreateRef<VulkanShader>(name, vertexSource, fragmentSource);
	}

	Ref<Shader> VulkanGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return CreateRef<VulkanShader>(name, vertexSource, geometrySource, fragmentSource);
	}

	Ref<Texture2D> VulkanGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return CreateRef<VulkanTexture2D>(spec);
	}

	Ref<Texture2D> VulkanGraphicsAPI::CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		return CreateRef<VulkanTexture2D>(filepath, descriptor);
	}

	Ref<Texture2DArray> VulkanGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return CreateRef<VulkanTexture2DArray>(spec);
	}

	Ref<UniformBuffer> VulkanGraphicsAPI::CreateUniformBuffer(size_t size, uint32_t binding)
	{
		return CreateRef<VulkanUniformBuffer>(size, binding);
	}
}
