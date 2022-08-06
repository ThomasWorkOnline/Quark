#include "qkpch.h"
#include "OpenALBuffer.h"
#include "OpenALAudioFormats.h"

#include "OpenALCore.h"

#include <fstream>

#define WAV_FORMAT_PCM         1
#define WAV_FORMAT_MULAW       6
#define WAV_FORMAT_ALAW        7
#define WAV_FORMAT_IBM_MULAW 257
#define WAV_FORMAT_IBM_ALAW  258
#define WAV_FORMAT_ADPCM     259

namespace Quark {

	struct WavHeader
	{
		/* RIFF Chunk Descriptor */
		uint8_t         RIFF[4];        // RIFF Header Magic header
		uint32_t        ChunkSize;      // RIFF Chunk Size
		uint8_t         WAVE[4];        // WAVE Header
		/* "fmt" sub-chunk */
		uint8_t         FMT[4];         // FMT header
		uint32_t        Subchunk1Size;  // Size of the fmt chunk
		uint16_t        AudioFormat;    // Audio format 1=PCM, 6=mulaw, 7=alaw,    257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
		uint16_t        Channels;       // Number of channels 1=Mono 2=Stereo
		uint32_t        Samplerate;     // Sampling Frequency in Hz
		uint32_t        BytesPerSec;    // bytes per second
		uint16_t        BlockAlign;     // 2=16-bit mono, 4=16-bit stereo
		uint16_t        BitDepth;       // Number of bits per sample
		/* "data" sub-chunk */
		uint8_t         DataChunkID[4]; // "data" string
		uint32_t        DataChunkSize;  // Sampled data length
	};

	namespace Utils {

		static constexpr ALenum GetOpenALFormat(uint16_t channels, uint16_t bitdepth)
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

		static WavHeader ReadWavHeader(std::ifstream& in)
		{
			WavHeader header{};
			in.read((char*)&header, sizeof(WavHeader));

			QK_RUNTIME_VERIFY(
				std::memcmp(header.RIFF, "RIFF", sizeof WavHeader::RIFF) == 0 &&
				std::memcmp(header.WAVE, "WAVE", sizeof WavHeader::WAVE) == 0 &&
				std::memcmp(header.FMT,  "fmt ", sizeof WavHeader::FMT) == 0 &&
				std::memcmp(header.DataChunkID, "data", sizeof WavHeader::DataChunkID) == 0,
				"Invalid or possibly corrupt .wav file");

			QK_RUNTIME_VERIFY(header.DataChunkSize != 0, "Invalid file contains no data");
			QK_RUNTIME_VERIFY(header.AudioFormat == WAV_FORMAT_PCM, "Quark does not support audio formats other than PCM");

			return header;
		}
	}

	OpenALAudioBuffer::OpenALAudioBuffer(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(filepath.rfind(".wav") != std::string::npos, "Quark only supports .wav formats");

		std::ifstream in(filepath.data(), std::ios::in | std::ios::binary);
		QK_CORE_ASSERT(in, "Could not open audio file: '{0}'", filepath);

		// Read the header
		WavHeader header = Utils::ReadWavHeader(in);

		m_Spec.Size = static_cast<size_t>(header.DataChunkSize);
		m_Spec.Samplerate = header.Samplerate;
		m_Spec.Format = GetFormat(header.Channels, header.BitDepth, false);

		// Read the data
		std::vector<char> audioData;
		audioData.reserve(header.DataChunkSize);

		in.read(audioData.data(), header.DataChunkSize);
		in.close();

		ALCALL(alGenBuffers(1, &m_BufferID));
		ALCALL(alBufferData(m_BufferID, Utils::GetOpenALFormat(header.Channels, header.BitDepth),
			audioData.data(), header.DataChunkSize, header.Samplerate));

		QK_CORE_ASSERT(alIsBuffer(m_BufferID), "Audio buffer is invalid");
	}

	OpenALAudioBuffer::OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data) : AudioBuffer(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(data, "Data must be a valid pointer");

		ALCALL(alGenBuffers(1, &m_BufferID));
		ALCALL(alBufferData(m_BufferID, AudioFormatToOpenALFormat(m_Spec.Format), data, (ALsizei)m_Spec.Size, m_Spec.Samplerate));

		QK_CORE_ASSERT(alIsBuffer(m_BufferID), "Audio buffer is invalid");
	}

	OpenALAudioBuffer::~OpenALAudioBuffer()
	{
		QK_PROFILE_FUNCTION();

		ALCALL(alDeleteBuffers(1, &m_BufferID));
	}

	void OpenALAudioBuffer::SetData(const void* data, size_t size)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(data, "Data must be a valid pointer");
		QK_CORE_ASSERT(size == m_Spec.Size, "Could not copy buffer, data size must equal specification");

		ALCALL(alBufferData(m_BufferID, AudioFormatToOpenALFormat(m_Spec.Format), data, (ALsizei)size, m_Spec.Samplerate));
	}
}
