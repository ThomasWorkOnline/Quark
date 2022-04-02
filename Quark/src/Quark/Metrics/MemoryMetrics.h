#pragma once

#include "Quark/Core/Core.h"
#include <atomic>

//          v--- define ENABLE_METRICS to enable memory profiling
#if defined(ENABLE_METRICS) || defined(QK_DEBUG)
#	define QK_MEMORY_METRICS
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

#ifdef QK_MEMORY_METRICS

[[nodiscard("return value of Quark operator new ignored")]]
void* operator new(size_t size);
void operator delete(void* block, size_t size) noexcept;

[[nodiscard("return value of Quark operator new[] ignored")]]
void* operator new[](size_t size);
void operator delete[](void* block, size_t size) noexcept;

#endif
