#pragma once

#include "Quark/Renderer/Font.h"
#include "OpenGLTexture.h"

namespace Quark {

	class OpenGLFont : public OpenGLTexture, public Font
	{
	public:
		OpenGLFont(std::string_view filepath, uint32_t fontSize);
		virtual ~OpenGLFont() final override;

		virtual const void* GetHandle() const final override { return static_cast<const OpenGLTexture*>(this); }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		OpenGLFont(const OpenGLFont&) = delete;
		OpenGLFont& operator=(const OpenGLFont&) = delete;
	};
}
