#pragma once

#include "Quark/Core/Core.h"
#include <atomic>

//          v--- define QK_ENABLE_ALL_METRICS to enable memory profiling
#if defined(QK_ENABLE_ALL_METRICS) || defined(QK_DEBUG)
#	define QK_ENABLE_MEMORY_METRICS 1
#endif

namespace Quark {

	struct MemoryMetrics
	{
		static inline std::atomic_size_t Allocations   = 0;
		static inline std::atomic_size_t Deallocations = 0;

		static inline std::atomic_size_t MemoryAllocated = 0;
		static inline std::atomic_size_t MemoryFreed     = 0;

		static size_t GetUsage() { return MemoryAllocated - MemoryFreed; }
	};
}

#if QK_ENABLE_MEMORY_METRICS

[[nodiscard]]
void* operator new(size_t size);
void operator delete(void* block, size_t size) noexcept;

[[nodiscard]]
void* operator new[](size_t size);
void operator delete[](void* block, size_t size) noexcept;

#endif
