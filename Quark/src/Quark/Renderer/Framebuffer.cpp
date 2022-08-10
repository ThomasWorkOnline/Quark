#include "qkpch.h"
#include "Framebuffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	FramebufferAttachment::FramebufferAttachment(const FramebufferAttachmentSpecification& spec)
		: m_Spec(spec)
	{
	}

	FramebufferAttachment* FramebufferAttachment::Create(void* image, const FramebufferAttachmentSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateFramebufferAttachment(image, spec);
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
	}

	Framebuffer* Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateFramebuffer(spec);
	}
}
