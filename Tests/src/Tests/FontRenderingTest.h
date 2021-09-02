#pragma once

#include <Quark.h>

using namespace Quark;

class FontRenderingTest : public Application
{
public:
	FontRenderingTest()
	{
		m_Font = Font::Create("Hello");
	}

private:
	Ref<Font> m_Font;
};
