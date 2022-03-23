#include "OpenALBuffer.h"
#include "OpenALAudioFormats.h"

#include "OpenALCore.h"

namespace Quark {

	OpenALAudioBuffer::OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data)
		: m_Spec(spec)
	{
		ALCALL(alGenBuffers(1, &m_BufferID));
		ALCALL(alBufferData(m_BufferID, GetAudioFormat(spec.Format), data, spec.Size, spec.Frequency));

		QK_ASSERT(alIsBuffer(m_BufferID), "Audio buffer is invalid");
	}

	OpenALAudioBuffer::~OpenALAudioBuffer()
	{
		ALCALL(alDeleteBuffers(1, &m_BufferID));
	}
}
