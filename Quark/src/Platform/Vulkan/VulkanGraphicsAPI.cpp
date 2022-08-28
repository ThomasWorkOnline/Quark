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
#include "VulkanSampler.h"
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

	Scope<CommandBuffer> VulkanGraphicsAPI::CreateCommandBuffer()
	{
		return CreateScope<VulkanCommandBuffer>(VulkanContext::GetCurrentDevice());
	}

	Scope<Pipeline> VulkanGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return CreateScope<VulkanPipeline>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<RenderPass> VulkanGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return CreateScope<VulkanRenderPass>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<VertexBuffer> VulkanGraphicsAPI::CreateVertexBuffer(const void* vertices, size_t size)
	{
		return CreateScope<VulkanVertexBuffer>(VulkanContext::GetCurrentDevice(), vertices, size);
	}

	Scope<VertexBuffer> VulkanGraphicsAPI::CreateVertexBuffer(size_t size)
	{
		return CreateScope<VulkanVertexBuffer>(VulkanContext::GetCurrentDevice(), size);
	}

	Scope<IndexBuffer> VulkanGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count)
	{
		return CreateScope<VulkanIndexBuffer>(VulkanContext::GetCurrentDevice(), indices, count);
	}

	Scope<IndexBuffer> VulkanGraphicsAPI::CreateIndexBuffer(uint32_t count)
	{
		return CreateScope<VulkanIndexBuffer>(VulkanContext::GetCurrentDevice(), count);
	}

	Scope<Cubemap> VulkanGraphicsAPI::CreateCubemap(const CubemapSpecification& spec)
	{
		return CreateScope<VulkanCubemap>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<Font> VulkanGraphicsAPI::CreateFont(std::string_view filepath, uint32_t fontSize)
	{
		return CreateScope<VulkanFont>(VulkanContext::GetCurrentDevice(), filepath, fontSize);
	}

	Scope<Framebuffer> VulkanGraphicsAPI::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return CreateScope<VulkanFramebuffer>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<FramebufferAttachment> VulkanGraphicsAPI::CreateFramebufferAttachment(const FramebufferAttachmentSpecification& spec)
	{
		return CreateScope<VulkanFramebufferAttachment>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<Shader> VulkanGraphicsAPI::CreateShader(std::string_view filepath)
	{
		return CreateScope<VulkanShader>(VulkanContext::GetCurrentDevice(), filepath);
	}

	Scope<Shader> VulkanGraphicsAPI::CreateShader(std::string_view name, const SpirvSource& vertexSource, const SpirvSource& fragmentSource)
	{
		return CreateScope<VulkanShader>(VulkanContext::GetCurrentDevice(), name, vertexSource, fragmentSource);
	}

	Scope<Shader> VulkanGraphicsAPI::CreateShader(std::string_view name, const SpirvSource& vertexSource, const SpirvSource& geometrySource, const SpirvSource& fragmentSource)
	{
		return CreateScope<VulkanShader>(VulkanContext::GetCurrentDevice(), name, vertexSource, geometrySource, fragmentSource);
	}

	Scope<Sampler2D> VulkanGraphicsAPI::CreateSampler2D(const Sampler2DSpecification& spec)
	{
		return CreateScope<VulkanSampler2D>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<Texture2D> VulkanGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return CreateScope<VulkanTexture2D>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<Texture2D> VulkanGraphicsAPI::CreateTexture2D(std::string_view filepath, const TextureRenderModes& renderModes)
	{
		return CreateScope<VulkanTexture2D>(VulkanContext::GetCurrentDevice(), filepath, renderModes);
	}

	Scope<Texture2DArray> VulkanGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return CreateScope<VulkanTexture2DArray>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<UniformBuffer> VulkanGraphicsAPI::CreateUniformBuffer(const UniformBufferSpecification& spec)
	{
		return CreateScope<VulkanUniformBuffer>(VulkanContext::GetCurrentDevice(), spec);
	}
}
