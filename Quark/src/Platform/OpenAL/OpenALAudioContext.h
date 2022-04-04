#pragma once

#include "Quark/Audio/AudioContext.h"

typedef struct ALCcontext ALCcontext;

namespace Quark {

	class OpenALAudioContext : public AudioContext
	{
	public:
		OpenALAudioContext(void* device);
		~OpenALAudioContext() override;

		void Init() override;

	private:
		void* m_DeviceHandle;
		ALCcontext* m_Context;
	};
}
