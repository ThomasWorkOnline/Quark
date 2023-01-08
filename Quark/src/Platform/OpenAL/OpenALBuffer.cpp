#include "qkpch.h"
#include "OpenALBuffer.h"
#include "OpenALAudioFormats.h"

#include "OpenALCore.h"

#include <cstdio>
#include <stdexcept>

enum WaveAudioFormat : uint16_t
{
	WAVE_FORMAT_PCM        = 0x0001,
	WAVE_FORMAT_IEEE_FLOAT = 0x0003,
	WAVE_FORMAT_ALAW       = 0x0006,
	WAVE_FORMAT_MULAW      = 0x0007,
	WAVE_FORMAT_EXTENSIBLE = 0xFFFE,

	WAVE_FORMAT_IBM_MULAW_EXT = 257,
	WAVE_FORMAT_IBM_ALAW_EXT  = 258,
	WAVE_FORMAT_ADPCM_EXT     = 259,

	WAVE_FORMAT_MAX_ENUM = UINT16_MAX
};

namespace Quark {

	// WAVE files often have information chunks that precede or follow the sound data (data chunk).
	// Some programs (naively) assume that for PCM data,
	// the preamble in the file header is exactly 44 bytes long
	// and that the rest of the file contains sound data. This is not a safe assumption.

	struct WavHeader
	{
		// Wave files have a master RIFF chunk which includes a WAVE identifier followed by sub-chunks.
		// The data is stored in little-endian byte order.

		// RIFF Chunk Descriptor
		uint8_t         RIFF[4];          // RIFF Header Magic header
		uint32_t        ChunkSize;        // RIFF Chunk Size
		uint8_t         WAVE[4];          // WAVE Header

		// The fmt specifies the format of the data.
		// There are 3 variants of the Format chunk for sampled data.
		// These differ in the extensions to the basic fmt chunk.

		// fmt Sub-Chunk
		uint8_t         FormatChunkID[4]; // FMT header
		uint32_t        FormatChunkSize;  // Size of the fmt chunk
		WaveAudioFormat AudioFormat;      // Audio format 1=PCM, 6=alaw, 7=mulaw,    257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
		uint16_t        Channels;         // Number of channels 1=Mono 2=Stereo
		uint32_t        Samplerate;       // Sampling Frequency in Hz
		uint32_t        BytesPerSec;      // bytes per second
		uint16_t        BlockAlign;       // 2=16-bit mono, 4=16-bit stereo
		uint16_t        BitDepth;         // Number of bits per sample

		// "data" Sub-Chunk
		uint8_t         DataChunkID[4];   // "data" string
		uint32_t        DataChunkSize;    // Sampled data length

		// Samples
		// 1 pad byte if DataChunkSize is odd
	};

	namespace Utils {

		static bool ValidateWavHeader(const WavHeader& header)
		{
			bool magicHeadersOk =
				std::memcmp(header.RIFF, "RIFF", sizeof WavHeader::RIFF) == 0 &&
				std::memcmp(header.WAVE, "WAVE", sizeof WavHeader::WAVE) == 0 &&
				std::memcmp(header.FormatChunkID, "fmt ", sizeof WavHeader::FormatChunkID) == 0 &&
				std::memcmp(header.DataChunkID, "data", sizeof WavHeader::DataChunkID) == 0;

			// Possible values for FormatChunkSize include 16, 18 and 40.
			// Currently, only 16 byte formats are supported.

			bool formatChunkOk = header.FormatChunkSize == 16 && header.AudioFormat == WAVE_FORMAT_PCM;
			bool dataChunkOk = header.DataChunkSize != 0;

			return dataChunkOk && formatChunkOk && magicHeadersOk;
		}
	}

	OpenALAudioBuffer::OpenALAudioBuffer(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		FILE* in = std::fopen(filepath.data(), "rb");
		QK_CORE_VERIFY(in, "Could not open audio file: '{0}'", filepath);

		std::vector<std::byte> audioData;

		try
		{
			WavHeader header;
			std::fread(&header, sizeof(WavHeader), 1, in);

			QK_CORE_VERIFY(Utils::ValidateWavHeader(header), "Invalid or possibly corrupt wave file");

			audioData.resize(header.DataChunkSize);
			size_t readSize = std::fread(audioData.data(), 1, header.DataChunkSize, in);
			audioData.resize(readSize);

			QK_CORE_VERIFY(readSize == header.DataChunkSize, "Invalid or possibly corrupt wave file");

			m_Spec.Samplerate = header.Samplerate;
			m_Spec.BitDepth = header.BitDepth;
			m_Spec.Channels = header.Channels;
			m_Spec.Format = GetFormat(header.Channels, header.BitDepth, false);

			uint32_t sampleCountPerChannel = static_cast<uint32_t>(readSize / (header.BitDepth >> 3) / header.Channels);
			m_Duration = (double)sampleCountPerChannel / (double)header.Samplerate;
		}
		catch (std::exception& e)
		{
			std::fclose(in);
			throw e;
		}

		std::fclose(in);

		ALCALL(alGenBuffers(1, &m_BufferID));
		ALCALL(alBufferData(m_BufferID, AudioFormatToOpenALFormat(m_Spec.Format), audioData.data(), (ALsizei)audioData.size(), m_Spec.Samplerate));

		QK_CORE_ASSERT(alIsBuffer(m_BufferID), "Audio buffer is invalid");
	}

	OpenALAudioBuffer::OpenALAudioBuffer(const AudioBufferSpecification& spec, const void* data) : AudioBuffer(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(data, "Data must be a valid pointer");

		ALsizei bufferSize = m_Spec.Samplerate * m_Spec.BitDepth * m_Spec.Channels;

		ALCALL(alGenBuffers(1, &m_BufferID));
		ALCALL(alBufferData(m_BufferID, AudioFormatToOpenALFormat(m_Spec.Format), data, bufferSize, m_Spec.Samplerate));

		QK_CORE_ASSERT(alIsBuffer(m_BufferID), "Audio buffer is invalid");
	}

	OpenALAudioBuffer::~OpenALAudioBuffer()
	{
		QK_PROFILE_FUNCTION();

		if (m_BufferID > 0)
			ALCALL(alDeleteBuffers(1, &m_BufferID));
	}

	void OpenALAudioBuffer::SetData(const void* data, size_t size)
	{
		QK_PROFILE_FUNCTION();

		ALsizei bufferSize = m_Spec.Samplerate * m_Spec.BitDepth * m_Spec.Channels;

		QK_CORE_ASSERT(data, "Data must be a valid pointer");
		QK_CORE_ASSERT(size == bufferSize, "Could not copy buffer, data size must equal specification");

		ALCALL(alBufferData(m_BufferID, AudioFormatToOpenALFormat(m_Spec.Format), data, (ALsizei)size, m_Spec.Samplerate));
	}
}
