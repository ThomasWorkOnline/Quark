#pragma once

#include "Quark/Audio/AudioContext.h"

typedef struct ALCcontext ALCcontext;

namespace Quark {

	class OpenALAudioContext : public AudioContext
	{
	public:
		OpenALAudioContext(void* device);
		virtual ~OpenALAudioContext() override;

		virtual void Init() override;

	private:
		void* m_DeviceHandle;
		ALCcontext* m_Context;
	};
}
