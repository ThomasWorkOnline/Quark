#include "OpenALAudioContext.h"

#include <AL/alc.h>

namespace Quark {

	OpenALAudioContext::OpenALAudioContext(void* device)
		: m_DeviceHandle(device), m_Context(nullptr)
	{
		QK_ASSERT(device, "Audio device is nullptr!");
	}

	OpenALAudioContext::~OpenALAudioContext()
	{
		QK_SCOPE_TIMER(OpenALAudioContext::~OpenALAudioContext);

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(m_Context);
	}

	void OpenALAudioContext::Init()
	{
		QK_SCOPE_TIMER(OpenALAudioContext::Init);

		m_Context = alcCreateContext(static_cast<ALCdevice*>(m_DeviceHandle), nullptr);

		ALCboolean result = alcMakeContextCurrent(m_Context);
		QK_ASSERT(result, "Failed to initialize OpenAL context");
		QK_CORE_TRACE("Created OpenAL audio context!");
	}
}
