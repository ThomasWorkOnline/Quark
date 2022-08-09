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

	Ref<CommandBuffer> VulkanGraphicsAPI::CreateCommandBuffer()
	{
		return CreateRef<VulkanCommandBuffer>(VulkanContext::GetCurrentDevice());
	}

	Ref<Pipeline> VulkanGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return CreateRef<VulkanPipeline>(VulkanContext::GetCurrentDevice(), spec);
	}

	Ref<RenderPass> VulkanGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return CreateRef<VulkanRenderPass>(VulkanContext::GetCurrentDevice(), spec);
	}

	Ref<VertexBuffer> VulkanGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return CreateRef<VulkanVertexBuffer>(VulkanContext::GetCurrentDevice(), vertices, size);
	}

	Ref<VertexBuffer> VulkanGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return CreateRef<VulkanVertexBuffer>(VulkanContext::GetCurrentDevice(), size);
	}

	Ref<IndexBuffer> VulkanGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return CreateRef<VulkanIndexBuffer>(VulkanContext::GetCurrentDevice(), indices, count);
	}

	Ref<IndexBuffer> VulkanGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return CreateRef<VulkanIndexBuffer>(VulkanContext::GetCurrentDevice(), count);
	}

	Ref<Cubemap> VulkanGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return CreateRef<VulkanCubemap>(VulkanContext::GetCurrentDevice(), spec);
	}

	Ref<Font> VulkanGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return CreateRef<VulkanFont>(VulkanContext::GetCurrentDevice(), filepath, fontSize);
	}

	Ref<Framebuffer> VulkanGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return CreateRef<VulkanFramebuffer>(VulkanContext::GetCurrentDevice(), spec);
	}

	Ref<FramebufferAttachment> VulkanGraphicsAPI::CreateFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec)
	{
		return CreateRef<VulkanFramebufferAttachment>(image, spec);
	}

	Ref<Shader> VulkanGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return CreateRef<VulkanShader>(VulkanContext::GetCurrentDevice(), filepath);
	}

	Ref<Shader> VulkanGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view fragmentSource)
	{
		return CreateRef<VulkanShader>(VulkanContext::GetCurrentDevice(), name, vertexSource, fragmentSource);
	}

	Ref<Shader> VulkanGraphicsAPI::CreateShader(std::string_view name, std::string_view vertexSource, std::string_view geometrySource, std::string_view fragmentSource)
	{
		return CreateRef<VulkanShader>(VulkanContext::GetCurrentDevice(), name, vertexSource, geometrySource, fragmentSource);
	}

	Ref<Texture2D> VulkanGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return CreateRef<VulkanTexture2D>(VulkanContext::GetCurrentDevice(), spec);
	}

	Ref<Texture2D> VulkanGraphicsAPI::CreateTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		return CreateRef<VulkanTexture2D>(VulkanContext::GetCurrentDevice(), filepath, descriptor);
	}

	Ref<Texture2DArray> VulkanGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return CreateRef<VulkanTexture2DArray>(VulkanContext::GetCurrentDevice(), spec);
	}

	Ref<UniformBuffer> VulkanGraphicsAPI::CreateUniformBuffer(size_t size, uint32_t binding)
	{
		return CreateRef<VulkanUniformBuffer>(VulkanContext::GetCurrentDevice(), size, binding);
	}
}
