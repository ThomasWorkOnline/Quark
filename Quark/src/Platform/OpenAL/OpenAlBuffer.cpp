#include "OpenALBuffer.h"
#include "OpenALAudioFormats.h"

#include <AL/al.h>

namespace Quark {

	OpenALAudioBuffer::OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data)
		: m_Spec(spec)
	{
		alGenBuffers(1, &m_AudioID);
		alBufferData(m_AudioID, GetAudioFormat(spec.Format), data, spec.Size, spec.Frequency);

		QK_ASSERT(alIsBuffer(m_AudioID), "Audio buffer is invalid");
	}

	OpenALAudioBuffer::~OpenALAudioBuffer()
	{
		alDeleteBuffers(1, &m_AudioID);
	}
}
