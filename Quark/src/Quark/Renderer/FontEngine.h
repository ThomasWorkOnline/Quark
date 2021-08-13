#pragma once

namespace Quark {

	class FontEngine
	{
	public:
		FontEngine() = delete;
		FontEngine operator= (const FontEngine& other) = delete;

		static void Initialize();
		static void Dispose();

	private:

	};
}
