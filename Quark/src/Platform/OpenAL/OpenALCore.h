#pragma once

#include "Quark/Core/Core.h"

#include <AL/al.h>

namespace Quark {

	static constexpr const char* OpenALErrorToString(ALenum err)
	{
		switch (err)
		{
			case AL_NO_ERROR:          return "AL_NO_ERROR";
			case AL_INVALID_NAME:      return "AL_INVALID_NAME";
			case AL_INVALID_ENUM:      return "AL_INVALID_ENUM";
			case AL_INVALID_VALUE:     return "AL_INVALID_VALUE";
			case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
			case AL_OUT_OF_MEMORY:     return "AL_OUT_OF_MEMORY";

			QK_ASSERT_NO_DEFAULT("Invalid error enum type!", nullptr);
		}
	}
}

#ifdef QK_DEBUG
#	define ALCALL(x) x; do { ALenum err = alGetError(); QK_CORE_ASSERT(err == AL_NO_ERROR, "{0} returned an error: {1}", #x, OpenALErrorToString(err)); } while (false)
#else
#	define ALCALL(x) x
#endif
