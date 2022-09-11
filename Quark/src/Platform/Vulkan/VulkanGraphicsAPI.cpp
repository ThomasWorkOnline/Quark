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
			m_Capabilities.FramebufferCapabilities.MaxWidth = props.limits.maxFramebufferWidth;
			m_Capabilities.FramebufferCapabilities.MaxHeight = props.limits.maxFramebufferHeight;
			m_Capabilities.FramebufferCapabilities.MaxAttachments = props.limits.maxColorAttachments;
		}

		// Sampler capabilities
		{
			m_Capabilities.SamplerCapabilities.MaxPerStageSamplers = props.limits.maxPerStageDescriptorSamplers;
		}

		// Uniform buffer capabilities
		{
			m_Capabilities.UniformBufferCapabilities.MaxBufferSize = props.limits.maxUniformBufferRange;
			m_Capabilities.UniformBufferCapabilities.MaxPerStageBuffers = props.limits.maxPerStageDescriptorUniformBuffers;
		}

		// Texture capabilities
		{
			m_Capabilities.TextureCapabilities.MaxWidth = props.limits.maxImageDimension1D;
			m_Capabilities.TextureCapabilities.MaxHeight = props.limits.maxImageDimension2D;
			m_Capabilities.TextureCapabilities.MaxDepth = props.limits.maxImageDimension3D;
			m_Capabilities.TextureCapabilities.MaxArrayLayers = props.limits.maxImageArrayLayers;
		}
	}

	VulkanGraphicsAPI::Version VulkanGraphicsAPI::GetDriverVersion() const
	{
		auto& props = VulkanContext::GetCurrentDevice()->GetPhysicalDeviceProperties();

		Version ver{};
		ver.Major = VK_API_VERSION_MAJOR(props.apiVersion);
		ver.Minor = VK_API_VERSION_MINOR(props.apiVersion);
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

	Scope<Sampler> VulkanGraphicsAPI::CreateSampler(const SamplerSpecification& spec)
	{
		return CreateScope<VulkanSampler>(VulkanContext::GetCurrentDevice(), spec);
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

	std::string VulkanGraphicsAPI::GetSpecification() const
	{
		auto ver = GetDriverVersion();
		auto& props = VulkanContext::GetCurrentDevice()->GetPhysicalDeviceProperties();

		std::stringstream ss;
		ss << "OpenGL Info:\n";
		ss << "|  " << "Vendor ID: " << props.vendorID << '\n';
		ss << "|  " << VulkanContext::GetCurrentDevice()->GetName() << '\n';
		ss << "|  " << ver.Major << '.' << ver.Minor << '\n';
		ss << "|  Per-stage Capabilities:\n";
		ss << "|    Max uniform buffers = " << m_Capabilities.UniformBufferCapabilities.MaxPerStageBuffers << '\n';
		ss << "|    Max samplers        = " << m_Capabilities.SamplerCapabilities.MaxPerStageSamplers;
		return ss.str();
	}
}
