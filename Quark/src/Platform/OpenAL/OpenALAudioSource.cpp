#include "qkpch.h"
#include "OpenALAudioSource.h"

#include "OpenALCore.h"
#include "OpenALBuffer.h"

namespace Quark {

	OpenALAudioSource::OpenALAudioSource()
	{
		ALCALL(alGenSources(1, &m_SourceID));
	}

	OpenALAudioSource::~OpenALAudioSource()
	{
		ALCALL(alDeleteSources(1, &m_SourceID));
	}

	void OpenALAudioSource::Play()
	{
		ALCALL(alSourcePlay(m_SourceID));
	}

	void OpenALAudioSource::Stop()
	{
		ALCALL(alSourceStop(m_SourceID));
	}

	void OpenALAudioSource::Pause()
	{
		ALCALL(alSourcePause(m_SourceID));
	}

	void OpenALAudioSource::SetBuffer(const AudioBuffer* buffer)
	{
		ALuint bufferID = static_cast<const OpenALAudioBuffer*>(buffer)->GetBufferID();
		ALCALL(alSourcei(m_SourceID, AL_BUFFER, bufferID));
	}
}
