#pragma once

#ifdef QK_DEBUG
#	define QK_DEBUG_CALL(x) x
#else
#	define QK_DEBUG_CALL(x)
#endif

#if defined(QK_DEBUG)
#	define QK_CONFIG_NAME "Debug"
#elif defined(QK_RELEASE)
#	define QK_CONFIG_NAME "Release"
#elif defined(QK_DIST)
#	define QK_CONFIG_NAME "Dist"
#else
#	define QK_CONFIG_NAME "Unknown configuration"
#endif

constexpr const char* GetConfigurationName()
{
	return QK_CONFIG_NAME;
}
