#include "qkpch.h"
#include "AudioBuffer.h"

#include "Platform/OpenAL/OpenALBuffer.h"

namespace Quark {

	AudioBuffer* AudioBuffer::Create(std::string_view filepath)
	{
		return new OpenALAudioBuffer(filepath);
	}

	AudioBuffer* AudioBuffer::Create(const AudioBufferSpecification& spec, const void* data)
	{
		return new OpenALAudioBuffer(spec, data);
	}
}
