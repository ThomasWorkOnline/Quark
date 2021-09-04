#pragma once

#include "../../Quark/Renderer/Font.h"

namespace Quark {

	class OpenGLFont : public Font
	{
	public:
		OpenGLFont(const std::string& filepath);
		virtual ~OpenGLFont();

		virtual bool operator==(const Font& other) const override
		{
			return false; // TODO: implement
		}

	private:
		void Init();
		void Shutdown();

	private:

	};
}
