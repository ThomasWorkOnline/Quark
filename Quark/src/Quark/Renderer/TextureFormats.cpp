#include "TextureFormats.h"

namespace Quark {

	bool IsformatUsingMips(TextureFilteringMode mode)
	{
		switch (mode)
		{
		case TextureFilteringMode::LinearMipmapLinear:
		case TextureFilteringMode::LinearMipmapNearest:
		case TextureFilteringMode::NearestMipmapLinear:
		case TextureFilteringMode::NearestMipmapNearest:
			return true;
		}

		return false;
	}
}
