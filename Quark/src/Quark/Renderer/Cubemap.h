#pragma once

#include "Quark/Core/Core.h"
#include "ColorFormats.h"
#include "TextureFormats.h"

namespace Quark {

	struct CubemapSpecification
	{
		uint32_t              Width = 0, Height = 0;
		ColorDataFormat       DataFormat     = ColorDataFormat::RGB;
		InternalColorFormat   InternalFormat = InternalColorFormat::RGB16f;
	};

	class Cubemap
	{
	public:
		virtual ~Cubemap() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual void SetData(uint32_t index, const void* data, size_t size) = 0;

		virtual bool operator==(const Cubemap& other) const = 0;

		static Ref<Cubemap> Create(const CubemapSpecification& spec);
	};
}
