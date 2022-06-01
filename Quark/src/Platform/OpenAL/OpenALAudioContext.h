#pragma once

#include "Quark/Audio/AudioContext.h"

typedef struct ALCcontext ALCcontext;
typedef struct ALCdevice  ALCdevice;

namespace Quark {

	class OpenALAudioContext final : public AudioContext
	{
	public:
		OpenALAudioContext(void* device);
		virtual ~OpenALAudioContext() override;

		virtual void Init() override;

	private:
		ALCdevice* m_DeviceHandle;
		ALCcontext* m_Context;
	};
}
