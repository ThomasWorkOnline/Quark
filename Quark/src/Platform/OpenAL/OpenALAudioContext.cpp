#include "qkpch.h"
#include "OpenALAudioContext.h"

#include "OpenALCore.h"
#include <AL/alc.h>

namespace Quark {

	OpenALAudioContext::OpenALAudioContext(void* device)
		: m_DeviceHandle(static_cast<ALCdevice*>(device)), m_Context(nullptr)
	{
		QK_CORE_ASSERT(device, "Audio device is nullptr!");
	}

	OpenALAudioContext::~OpenALAudioContext()
	{
		QK_PROFILE_FUNCTION();

		ALCALL(alcMakeContextCurrent(nullptr));
		ALCALL(alcDestroyContext(m_Context));
	}

	void OpenALAudioContext::Init()
	{
		QK_PROFILE_FUNCTION();

		m_Context = ALCALL(alcCreateContext(m_DeviceHandle, nullptr));

		ALCboolean result = ALCALL(alcMakeContextCurrent(m_Context));
		QK_CORE_ASSERT(result, "Failed to initialize OpenAL context");
		QK_CORE_TRACE("Created OpenAL audio context!");
	}
}
