#include "qkpch.h"
#include "OpenALAudioContext.h"

#include "OpenALCore.h"
#include <AL/alc.h>

namespace Quark {

	OpenALAudioContext::OpenALAudioContext(void* device)
		: m_DeviceHandle(device), m_Context(nullptr)
	{
		QK_CORE_ASSERT(device, "Audio device is nullptr!");
	}

	OpenALAudioContext::~OpenALAudioContext()
	{
		QK_SCOPE_TIMER(OpenALAudioContext::~OpenALAudioContext);

		ALCALL(alcMakeContextCurrent(nullptr));
		ALCALL(alcDestroyContext(m_Context));
	}

	void OpenALAudioContext::Init()
	{
		QK_SCOPE_TIMER(OpenALAudioContext::Init);

		m_Context = ALCALL(alcCreateContext(static_cast<ALCdevice*>(m_DeviceHandle), nullptr));

		ALCboolean result = ALCALL(alcMakeContextCurrent(m_Context));
		QK_CORE_ASSERT(result, "Failed to initialize OpenAL context");
		QK_CORE_TRACE("Created OpenAL audio context!");
	}
}
