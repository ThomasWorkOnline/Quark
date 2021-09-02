#pragma once

#include "../Core/Core.h"

namespace Quark {

	class Resource
	{
	public:
		Resource();
		virtual ~Resource() = default;

		uint32_t GetID() const { return m_ID; };

		virtual bool operator==(const Resource& other) const = 0;

	private:
		uint32_t m_ID;
	};
}
