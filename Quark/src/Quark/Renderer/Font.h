#pragma once

#include "../Core/Core.h"

#include "DeferredObject.h"

#include <string>
#include <unordered_map>

namespace Quark {

	struct Character
	{
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		glm::ivec2 Advance;
		float TexCoordX;
	};

	class Font : public DeferredObject
	{
	public:
		virtual ~Font() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual const Character& GetCharacter(uint32_t charcode) const = 0;
		virtual uint32_t GetCharacterCount() const = 0;

		virtual uint32_t GetPixelWidth() const = 0;
		virtual uint32_t GetPixelHeight() const = 0;

		virtual uint32_t GetAtlasWidth() const = 0;
		virtual uint32_t GetAtlasHeight() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const Font& other) const = 0;

		static Ref<Font> Create(const std::string& filepath, uint32_t width = 0, uint32_t height = 48);
	};

	class FontLibrary
	{
	public:
		Ref<Font> Load(const std::string& name, const std::string& filepath, uint32_t width = 0, uint32_t height = 48);
		void Add(const Ref<Font>& font, const std::string& name);

		const Ref<Font>& Get(const std::string& name);

		bool Exists(const std::string& name) const;
		size_t Size() const { return m_Fonts.size(); }

	private:
		std::unordered_map<std::string, Ref<Font>> m_Fonts;
	};
}
