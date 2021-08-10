#pragma once

#include "../Core/Core.h"

namespace Quark {

	class Resource
	{
	public:
		Resource();
		virtual ~Resource();

		virtual bool operator==(const Resource& other) const = 0;

	private:

	};
}
