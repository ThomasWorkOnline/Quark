#pragma once

#include "../Core/Core.h"

namespace Entropy {

	class Resource
	{
	public:
		virtual bool operator==(const Resource& other) const = 0;

	private:

	};
}
