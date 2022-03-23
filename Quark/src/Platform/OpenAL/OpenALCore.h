#pragma once

#include "Quark/Core/Core.h"

#include <AL/al.h>
#include <AL/alc.h>

#ifdef QK_DEBUG
#	define ALCALL(x) x; QK_CORE_ASSERT(alGetError() == AL_NO_ERROR, #x " returned an error!")
#else
#	define ALCALL(x) x
#endif
