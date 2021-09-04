#pragma once

#include <Quark.h>

using namespace Quark;

class FontRenderingTest : public Application
{
public:
	FontRenderingTest()
	{
		m_Font = Font::Create("Hello");
		m_Vao = VertexArray::Create();
	}

private:
	Ref<Font> m_Font;
	Ref<VertexArray> m_Vao;
};
