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

#include <vulkan/vulkan.hpp>

namespace Quark {

	void VulkanGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();

		m_Constraints.TextureConstraints.MaxTextureSlots = 32; // TODO: implement
	}

	void VulkanGraphicsAPI::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
	}

	void VulkanGraphicsAPI::EndRenderPass()
	{
	}

	void VulkanGraphicsAPI::Draw(uint32_t offset, uint32_t count)
	{
	}

	void VulkanGraphicsAPI::DrawIndexed(uint32_t indexCount)
	{
	}

	Ref<CommandBuffer> VulkanGraphicsAPI::CreateCommandBuffer()
	{
		return CreateRef<VulkanCommandBuffer>();
	}

	Ref<Pipeline> VulkanGraphicsAPI::CreatePipeline(const PipelineSpecification& spec)
	{
		return CreateRef<VulkanPipeline>(spec);
	}

	Ref<RenderPass> VulkanGraphicsAPI::CreateRenderPass(const RenderPassSpecification& spec)
	{
		return CreateRef<VulkanRenderPass>(spec);
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
