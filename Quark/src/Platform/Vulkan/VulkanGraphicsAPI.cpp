#include "qkpch.h"
#include "VulkanGraphicsAPI.h"

#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCubemap.h"
#include "VulkanFont.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanTextureArray.h"
#include "VulkanUniformBuffer.h"

#include <vulkan/vulkan.h>

namespace Quark {

	void VulkanGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();

		m_Capabilities.TextureConstraints.MaxTextureSlots = 32; // TODO: implement caps
	}

	CommandBuffer* VulkanGraphicsAPI::CreateCommandBuffer()
	{
		return new VulkanCommandBuffer(VulkanContext::GetCurrentDevice());
	}

	Pipeline* VulkanGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return new VulkanPipeline(VulkanContext::GetCurrentDevice(), spec);
	}

	RenderPass* VulkanGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return new VulkanRenderPass(VulkanContext::GetCurrentDevice(), spec);
	}

	VertexBuffer* VulkanGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return new VulkanVertexBuffer(VulkanContext::GetCurrentDevice(), vertices, size);
	}

	VertexBuffer* VulkanGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return new VulkanVertexBuffer(VulkanContext::GetCurrentDevice(), size);
	}

	IndexBuffer* VulkanGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return new VulkanIndexBuffer(VulkanContext::GetCurrentDevice(), indices, count);
	}

	IndexBuffer* VulkanGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return new VulkanIndexBuffer(VulkanContext::GetCurrentDevice(), count);
	}

	Cubemap* VulkanGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return new VulkanCubemap(VulkanContext::GetCurrentDevice(), spec);
	}

	Font* VulkanGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return new VulkanFont(VulkanContext::GetCurrentDevice(), filepath, fontSize);
	}

	Framebuffer* VulkanGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return new VulkanFramebuffer(VulkanContext::GetCurrentDevice(), spec);
	}

	FramebufferAttachment* VulkanGraphicsAPI::CreateFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec)
	{
		return new VulkanFramebufferAttachment(image, spec);
	}

	Shader* VulkanGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return new VulkanShader(VulkanContext::GetCurrentDevice(), filepath);
	}

	Shader* VulkanGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return new VulkanShader(VulkanContext::GetCurrentDevice(), name, vertexSource, fragmentSource);
	}

	Shader* VulkanGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return new VulkanShader(VulkanContext::GetCurrentDevice(), name, vertexSource, geometrySource, fragmentSource);
	}

	Texture2D* VulkanGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return new VulkanTexture2D(VulkanContext::GetCurrentDevice(), spec);
	}

	Texture2D* VulkanGraphicsAPI::CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		return new VulkanTexture2D(VulkanContext::GetCurrentDevice(), filepath, descriptor);
	}

	Texture2DArray* VulkanGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return new VulkanTexture2DArray(VulkanContext::GetCurrentDevice(), spec);
	}

	UniformBuffer* VulkanGraphicsAPI::CreateUniformBuffer(size_t size, uint32_t binding)
	{
		return new VulkanUniformBuffer(VulkanContext::GetCurrentDevice(), size, binding);
	}
}
