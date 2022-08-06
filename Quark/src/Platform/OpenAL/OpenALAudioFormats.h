#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Audio/AudioFormats.h"

#include <AL/al.h>

namespace Quark {

	constexpr ALenum AudioFormatToOpenALFormat(AudioFormat format)
	{
		switch (format.Channels)
		{
			case ChannelsFormat::Mono:
				switch (format.BitDepth)
				{
					case BitDepthFormat::Int8:		return AL_FORMAT_MONO8;
					case BitDepthFormat::Int16:		return AL_FORMAT_MONO16;
					default:
						QK_CORE_FATAL("Invalid bit depth");
						return AL_NONE;
				}
			case ChannelsFormat::Stereo:
				switch (format.BitDepth)
				{
					case BitDepthFormat::Int8:		return AL_FORMAT_STEREO8;
					case BitDepthFormat::Int16:		return AL_FORMAT_STEREO16;
					default:
						QK_CORE_FATAL("Invalid bit depth");
						return AL_NONE;
				}
			default:
				QK_CORE_FATAL("Invalid audio format");
				return AL_NONE;
		}
	}
}
