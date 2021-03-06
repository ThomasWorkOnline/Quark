#pragma once

#include "../Core/Core.h"

namespace Entropy {

	class Ressource
	{
	public:
		virtual bool operator==(const Ressource& other) const = 0;

	private:

	};
}
