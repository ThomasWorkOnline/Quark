#pragma once

#include "Quark/Core/Core.h"
#include <atomic>

//          v--- define QK_ENABLE_ALL_METRICS to enable memory profiling (see Core/Tweaks.h)
#if defined(QK_ENABLE_ALL_METRICS) || defined(QK_DEBUG) || defined(QK_RELEASE)
#	ifndef QK_ENABLE_MEMORY_METRICS
#		define QK_ENABLE_MEMORY_METRICS
#	endif
#endif

namespace Quark {

	struct MemoryMetrics
	{
		static inline std::atomic_size_t Allocations   = 0;
		static inline std::atomic_size_t Deallocations = 0;
		static inline std::atomic_size_t UntrackedDeallocations = 0;

		static inline std::atomic_size_t MemoryAllocated = 0;
		static inline std::atomic_size_t MemoryFreed     = 0;

		static std::size_t GetUsage() { return MemoryAllocated - MemoryFreed; }
		static std::size_t ActiveBlocks() { return Allocations - Deallocations - UntrackedDeallocations; }
	};
}
