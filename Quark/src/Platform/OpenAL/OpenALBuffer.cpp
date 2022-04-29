#include "qkpch.h"
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
					QK_CORE_FATAL("Invalid bitdepth");
					return AL_NONE;
			}
			case 2:
			switch (bitdepth)
			{
				case 8:  return AL_FORMAT_STEREO8;
				case 16: return AL_FORMAT_STEREO16;
				default:
					QK_CORE_FATAL("Invalid bitdepth");
					return AL_NONE;
			}
			default:
				QK_CORE_FATAL("Unknown audio format");
				return AL_NONE;
		}
	}

	OpenALAudioBuffer::OpenALAudioBuffer(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(filepath.rfind(".wav") != std::string::npos, "Quark only supports .wav formats");

		WavHeader header{};
		std::ifstream in(filepath.data(), std::ios::in | std::ios::binary);
		QK_CORE_ASSERT(in.is_open(), "Could not open audio file: '{0}'", filepath);

		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		in.seekg(0, std::ios::beg);

		QK_CORE_ASSERT(size >= sizeof(WavHeader), "Error parsing file: DataChunkSize is set to zero");

		// Read the header
		in.read((char*)&header, sizeof(WavHeader));

		size_t dataSize = size - sizeof(WavHeader);
		QK_CORE_ASSERT(dataSize != 0, "Invalid file contains no data");
		QK_CORE_ASSERT(header.AudioFormat == 1, "Quark does not support audio formats other than PCM");

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
		QK_PROFILE_FUNCTION();

		ALCALL(alGenBuffers(1, &m_BufferID));
		ALCALL(alBufferData(m_BufferID, GetAudioFormat(spec.Format), data, (ALsizei)spec.Size, spec.Samplerate));

		QK_CORE_ASSERT(alIsBuffer(m_BufferID), "Audio buffer is invalid");
	}

	OpenALAudioBuffer::~OpenALAudioBuffer()
	{
		QK_PROFILE_FUNCTION();

		ALCALL(alDeleteBuffers(1, &m_BufferID));
	}
}
