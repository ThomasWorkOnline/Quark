#include "AudioBuffer.h"

#include "../../Platform/OpenAL/OpenALBuffer.h"

namespace Quark {

    Ref<AudioBuffer> AudioBuffer::Create(const AudioBufferSpecification& spec, const void* data)
    {
        return CreateRef<OpenALAudioBuffer>(spec, data);
    }
}
