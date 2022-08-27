#include "qkpch.h"
#include "Framebuffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	FramebufferAttachment::FramebufferAttachment(const FramebufferAttachmentSpecification& spec)
		: m_Spec(spec)
	{
	}

	Scope<FramebufferAttachment> FramebufferAttachment::Create(const FramebufferAttachmentSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateFramebufferAttachment(spec);
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
	}

	Scope<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateFramebuffer(spec);
	}
}
