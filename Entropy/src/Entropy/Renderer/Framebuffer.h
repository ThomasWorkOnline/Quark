#pragma once

#include "../Core/Core.h"

namespace Entropy {

	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color formats
		RGBA8,

		// Depth formats
		Depth24Stencil8,

		Default = RGBA8
	};

	enum class FramebufferFilteringFormat
	{
		None = 0,

		// Filter formats
		Nearest,
		Linear,

		Default = Linear
	};

	enum class FramebufferTilingFormat
	{
		None = 0,

		// Tiling formats
		// TODO:

		Clamp,
		Repeat,

		Default = Clamp
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat textureFormat,
			FramebufferFilteringFormat filteringFormat = FramebufferFilteringFormat::Default,
			FramebufferTilingFormat tilingFormat = FramebufferTilingFormat::Default)
			: TextureFormat(textureFormat), FilteringFormat(filteringFormat), TilingFormat(tilingFormat) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		FramebufferFilteringFormat FilteringFormat = FramebufferFilteringFormat::Default;
		FramebufferTilingFormat TilingFormat = FramebufferTilingFormat::Default;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Attach() = 0;
		virtual void Detach() = 0;

		virtual void AttachColorAttachment(uint32_t textureSlot, uint32_t index = 0) = 0;
		virtual void AttachDepthAttachment(uint32_t textureSlot) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}
