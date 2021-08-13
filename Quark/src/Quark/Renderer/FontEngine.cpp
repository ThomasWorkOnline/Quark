#include "FontEngine.h"

#include "../Core/Core.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Quark {

	static FT_Library s_Library;

	void FontEngine::Initialize()
	{
		QK_TIME_SCOPE_DEBUG(FontEngine::Initialize);

		FT_Error error = FT_Init_FreeType(&s_Library);
		QK_ASSERT(error == FT_Err_Ok, "Could not initialize freetype!");
	}

	void FontEngine::Dispose()
	{

	}
}
