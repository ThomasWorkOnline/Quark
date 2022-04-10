#include "AudioBuffer.h"

#include "Platform/OpenAL/OpenALBuffer.h"

namespace Quark {

	Ref<AudioBuffer> AudioBuffer::Create(std::string_view filepath)
	{
		return CreateRef<OpenALAudioBuffer>(filepath);
	}

	Ref<AudioBuffer> AudioBuffer::Create(const AudioBufferSpecification& spec, const void* data)
	{
		return CreateRef<OpenALAudioBuffer>(spec, data);
	}
}
