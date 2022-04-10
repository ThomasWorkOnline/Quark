#include "OpenALBuffer.h"
#include "OpenALAudioFormats.h"

#include "OpenALCore.h"
#include "OpenALUtilities.h"

#include <fstream>

namespace Quark {

	static constexpr ALenum GetFormat(uint16_t channels, uint16_t bitdepth)
	{
		switch (channels)
		{
			case 1:
			switch (bitdepth)
			{
				case 8:  return AL_FORMAT_MONO8;
				case 16: return AL_FORMAT_MONO16;
				default:
					QK_FATAL("Invalid bitdepth");
					return AL_NONE;
			}
			case 2:
			switch (bitdepth)
			{
				case 8:  return AL_FORMAT_STEREO8;
				case 16: return AL_FORMAT_STEREO16;
				default:
					QK_FATAL("Invalid bitdepth");
					return AL_NONE;
			}
			default:
				QK_FATAL("Unknown audio format");
				return AL_NONE;
		}
	}

	OpenALAudioBuffer::OpenALAudioBuffer(std::string_view filepath)
	{
		QK_SCOPE_TIMER(OpenALAudioBuffer::OpenALAudioBuffer);
		QK_CORE_ASSERT(filepath.rfind(".wav") != std::string::npos, "Quark only supports .wav formats");

        WavHeader header{};
        std::ifstream in(filepath.data(), std::ios::in | std::ios::binary);
        QK_CORE_ASSERT(in.is_open(), "could not open file {0}", filepath);

		// TODO: use a memory mapped file for performance

		// Read the header
		in.read((char*)&header, sizeof(WavHeader));

		QK_CORE_ASSERT(header.AudioFormat == 1, "Quark does not support audio formats other than PCM!");

		// Read the data
		char* data = new char[header.DataChunkSize];

		in.read(data, header.DataChunkSize);
		in.close();

		ALCALL(alGenBuffers(1, &m_BufferID));
		ALCALL(alBufferData(m_BufferID,
			GetFormat(header.Channels, header.BitDepth),
			data, header.DataChunkSize, header.Samplerate));

		delete[] data;
	}

	OpenALAudioBuffer::OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data)
		: m_Spec(spec)
	{
		QK_SCOPE_TIMER(OpenALAudioBuffer::OpenALAudioBuffer);

		ALCALL(alGenBuffers(1, &m_BufferID));
		ALCALL(alBufferData(m_BufferID, GetAudioFormat(spec.Format), data, spec.Size, spec.Samplerate));

		QK_CORE_ASSERT(alIsBuffer(m_BufferID), "Audio buffer is invalid");
	}

	OpenALAudioBuffer::~OpenALAudioBuffer()
	{
		ALCALL(alDeleteBuffers(1, &m_BufferID));
	}
}
