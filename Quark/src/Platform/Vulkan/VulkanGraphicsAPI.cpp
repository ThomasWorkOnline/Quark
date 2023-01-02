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
#include "VulkanUniformBuffer.h"

#include <vulkan/vulkan.h>

namespace Quark {

	void VulkanGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();

		auto& props = VulkanContext::GetCurrentDevice()->GetPhysicalDeviceProperties();

		// Framebuffer capabilities
		{
			m_Capabilities.Framebuffer.MaxWidth = props.limits.maxFramebufferWidth;
			m_Capabilities.Framebuffer.MaxHeight = props.limits.maxFramebufferHeight;
			m_Capabilities.Framebuffer.MaxLayers = props.limits.maxFramebufferLayers;
			m_Capabilities.Framebuffer.MaxAttachments = props.limits.maxColorAttachments;

			VkSampleCountFlags samples = props.limits.framebufferColorSampleCounts & props.limits.framebufferDepthSampleCounts;
			m_Capabilities.Framebuffer.MaxMSAA = GetHighestSampleCount(samples);
		}

		// Sampler capabilities
		{
			m_Capabilities.Sampler.MaxTextureUnits = 32; // TODO: fix
			m_Capabilities.Sampler.MaxAnisotropy = props.limits.maxSamplerAnisotropy;
		}

		// Uniform buffer capabilities
		{
			m_Capabilities.UniformBuffer.MaxBufferSize = props.limits.maxUniformBufferRange;
			m_Capabilities.UniformBuffer.MaxPerStageBuffers = props.limits.maxPerStageDescriptorUniformBuffers;
		}

		// Texture capabilities
		{
			m_Capabilities.Texture.Max2DSize = props.limits.maxImageDimension2D;
			m_Capabilities.Texture.Max3DSize = props.limits.maxImageDimension3D;
			m_Capabilities.Texture.MaxArrayLayers = props.limits.maxImageArrayLayers;
			m_Capabilities.Texture.MaxCubemapSize = props.limits.maxImageDimensionCube;
		}
	}

	RHIVersion VulkanGraphicsAPI::GetRHIVersion() const
	{
		auto& props = VulkanContext::GetCurrentDevice()->GetPhysicalDeviceProperties();

		// TODO: get driver version instead of api version
		// VkPhysicalDeviceDriverProperties props;

		RHIVersion ver{};
		ver.Major = VK_API_VERSION_MAJOR(props.apiVersion);
		ver.Minor = VK_API_VERSION_MINOR(props.apiVersion);
		ver.Patch = VK_API_VERSION_PATCH(props.apiVersion);
		return ver;
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

	Scope<IndexBuffer> VulkanGraphicsAPI::CreateIndexBuffer(const uint32_t* indices, uint32_t count, IndexType indexType)
	{
		return CreateScope<VulkanIndexBuffer>(VulkanContext::GetCurrentDevice(), indices, count, indexType);
	}

	Scope<IndexBuffer> VulkanGraphicsAPI::CreateIndexBuffer(uint32_t count, IndexType indexType)
	{
		return CreateScope<VulkanIndexBuffer>(VulkanContext::GetCurrentDevice(), count, indexType);
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

	Scope<Shader> VulkanGraphicsAPI::CreateShader(
		std::string_view name,
		std::string_view vertexSource,
		std::string_view fragmentSource)
	{
		return CreateScope<VulkanShader>(VulkanContext::GetCurrentDevice(), name, vertexSource, fragmentSource);
	}

	Scope<Shader> VulkanGraphicsAPI::CreateShader(
		std::string_view name,
		std::string_view vertexSource,
		std::string_view geometrySource,
		std::string_view fragmentSource)
	{
		return CreateScope<VulkanShader>(VulkanContext::GetCurrentDevice(), name, vertexSource, geometrySource, fragmentSource);
	}

	Scope<Shader> VulkanGraphicsAPI::CreateShader(
		std::string_view name,
		std::span<const uint32_t> vertexSpirv,
		std::span<const uint32_t> fragmentSpirv)
	{
		return CreateScope<VulkanShader>(VulkanContext::GetCurrentDevice(), name, vertexSpirv, fragmentSpirv);
	}

	Scope<Shader> VulkanGraphicsAPI::CreateShader(
		std::string_view name,
		std::span<const uint32_t> vertexSpirv,
		std::span<const uint32_t> geometrySpirv,
		std::span<const uint32_t> fragmentSpirv)
	{
		return CreateScope<VulkanShader>(VulkanContext::GetCurrentDevice(), name, vertexSpirv, geometrySpirv, fragmentSpirv);
	}

	Scope<Sampler> VulkanGraphicsAPI::CreateSampler(const SamplerSpecification& spec)
	{
		return CreateScope<VulkanSampler>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<Texture2D> VulkanGraphicsAPI::CreateTexture2D(const Texture2DSpecification& spec)
	{
		return CreateScope<VulkanTexture2D>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<Texture2D> VulkanGraphicsAPI::CreateTexture2D(std::string_view filepath)
	{
		return CreateScope<VulkanTexture2D>(VulkanContext::GetCurrentDevice(), filepath);
	}

	Scope<Texture2DArray> VulkanGraphicsAPI::CreateTexture2DArray(const Texture2DArraySpecification& spec)
	{
		return CreateScope<VulkanTexture2DArray>(VulkanContext::GetCurrentDevice(), spec);
	}

	Scope<UniformBuffer> VulkanGraphicsAPI::CreateUniformBuffer(size_t size)
	{
		return CreateScope<VulkanUniformBuffer>(VulkanContext::GetCurrentDevice(), size);
	}

	const char* VulkanGraphicsAPI::GetDeviceName() const
	{
		return VulkanContext::GetCurrentDevice()->GetName();
	}

	std::string VulkanGraphicsAPI::GetSpecification() const
	{
		auto ver = GetRHIVersion();
		auto& props = VulkanContext::GetCurrentDevice()->GetPhysicalDeviceProperties();

		std::stringstream ss;
		ss << "OpenGL Info:\n";
		ss << "|  " << "Vendor ID: " << props.vendorID << '\n';
		ss << "|  " << VulkanContext::GetCurrentDevice()->GetName() << '\n';
		ss << "|  " << ver.Major << '.' << ver.Minor << '\n';
		ss << "|  Per-stage Capabilities:\n";
		ss << "|    Max uniform buffers = " << m_Capabilities.UniformBuffer.MaxPerStageBuffers << '\n';
		ss << "|    Max samplers        = " << m_Capabilities.Sampler.MaxTextureUnits;
		return ss.str();
	}
}
