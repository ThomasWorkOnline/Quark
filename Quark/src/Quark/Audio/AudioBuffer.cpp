#include "qkpch.h"
#include "AudioBuffer.h"

#include "Platform/OpenAL/OpenALBuffer.h"

namespace Quark {

	Scope<AudioBuffer> AudioBuffer::Create(std::string_view filepath)
	{
		return CreateScope<OpenALAudioBuffer>(filepath);
	}

	Scope<AudioBuffer> AudioBuffer::Create(const AudioBufferSpecification& spec, const void* data)
	{
		return CreateScope<OpenALAudioBuffer>(spec, data);
	}
}
