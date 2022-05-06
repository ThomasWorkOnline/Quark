#pragma once

#include "Quark/Audio/AudioContext.h"

typedef struct ALCcontext ALCcontext;
typedef struct ALCdevice  ALCdevice;

namespace Quark {

	class OpenALAudioContext : public AudioContext
	{
	public:
		OpenALAudioContext(void* device);
		~OpenALAudioContext() override;

		void Init() override;

	private:
		ALCdevice* m_DeviceHandle;
		ALCcontext* m_Context;
	};
}
