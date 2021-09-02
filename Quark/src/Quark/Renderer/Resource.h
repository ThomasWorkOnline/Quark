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

		static void Hold(const Ref<Resource>& res);

		template<typename T, typename ... Args>
		static Ref<T> Create(Args&& ... args)
		{
			auto ref = CreateRef<T>(std::forward<Args>(args)...);
			Hold(ref);
			return ref;
		}

	private:
		uint32_t m_ID;
	};
}
