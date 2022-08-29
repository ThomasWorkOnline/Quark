#include "qkpch.h"
#include "Framebuffer.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	FramebufferAttachment::FramebufferAttachment(const FramebufferAttachmentSpecification& spec)
		: m_Spec(spec)
	{
	}

	Scope<FramebufferAttachment> FramebufferAttachment::Create(const FramebufferAttachmentSpecification& spec)
	{
		return s_GraphicsAPI->CreateFramebufferAttachment(spec);
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
	}

	Scope<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return s_GraphicsAPI->CreateFramebuffer(spec);
	}
}
