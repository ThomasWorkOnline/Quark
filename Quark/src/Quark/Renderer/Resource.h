#pragma once

#include "../Core/Core.h"

namespace Quark {

	class Resource
	{
	public:
		virtual bool operator==(const Resource& other) const = 0;

	private:

	};
}
