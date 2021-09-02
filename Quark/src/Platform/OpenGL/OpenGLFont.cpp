#include "OpenGLFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Quark {

	static uint32_t s_FontCount = 0;

	static FT_Library s_Library;

	OpenGLFont::OpenGLFont(const std::string& filepath)
	{
		if (s_FontCount == 0)
			Init();

		s_FontCount++;
	}

	OpenGLFont::~OpenGLFont()
	{
		--s_FontCount;

		if (s_FontCount == 0)
			Shutdown();
	}

	void OpenGLFont::Init()
	{
		QK_TIME_SCOPE_DEBUG(OpenGLFont::Init);

		FT_Error error = FT_Init_FreeType(&s_Library);
		QK_ASSERT(error == FT_Err_Ok, "Could not initialize freetype!");
	}

	void OpenGLFont::Shutdown()
	{
		QK_TIME_SCOPE_DEBUG(OpenGLFont::Shutdown);

		delete s_Library;
	}
}
