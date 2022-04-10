#pragma once

#include "Quark/Core/Core.h"

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
        uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
        uint16_t        Channels;       // Number of channels 1=Mono 2=Sterio
        uint32_t        Samplerate;     // Sampling Frequency in Hz
        uint32_t        BytesPerSec;    // bytes per second
        uint16_t        BlockAlign;     // 2=16-bit mono, 4=16-bit stereo
        uint16_t        BitDepth;       // Number of bits per sample
        /* "data" sub-chunk */
        uint8_t         DataChunkID[4]; // "data" string
        uint32_t        DataChunkSize;  // Sampled data length
    };
}
