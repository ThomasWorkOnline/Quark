#include "qkpch.h"
#include "Core.h"

#include <fpng/fpng.h>

namespace Quark {

	namespace RuntimeCore {

		void Init()
		{
			fpng::fpng_init();
		}

		void Shutdown()
		{
		}
	}
}
