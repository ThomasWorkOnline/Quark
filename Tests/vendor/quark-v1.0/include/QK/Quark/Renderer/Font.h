#pragma once

#include "../Core/Core.h"

#include <string>
#include <unordered_map>

namespace Quark {

	class Font
	{
	public:
		virtual ~Font() = default;

		static Ref<Font> Create(const std::string& filepath);
	};

	class FontLibrary
	{
	public:
		Ref<Font> Load(const std::string& name, const std::string& filepath);
		void Add(const Ref<Font>& font, const std::string& name);

		const Ref<Font>& Get(const std::string& name);

		bool Exists(const std::string& name) const;
		size_t Size() const { return m_Fonts.size(); }

	private:
		std::unordered_map<std::string, Ref<Font>> m_Fonts;
	};

	class FontLoader
	{
	public:
		FontLoader() = delete;
		FontLoader operator= (const FontLoader& other) = delete;

		static void Initialize();
		static void Dispose();
	};
}
