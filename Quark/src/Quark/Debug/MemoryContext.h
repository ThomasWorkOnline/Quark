#pragma once

#include "Quark/Core/Core.h"

#include <atomic>
#include <mutex>
#include <unordered_map>

//          v--- define QK_ENABLE_ALL_METRICS to enable memory profiling (see Core/Tweaks.h)
#if defined(_MSC_VER) && (defined(QK_ENABLE_ALL_METRICS) || defined(QK_DEBUG))
#	ifndef QK_ENABLE_MEMORY_METRICS
#		define QK_ENABLE_MEMORY_METRICS
#	endif
#endif

namespace Quark {

	template<class T>
	struct MemoryTrackerAllocator
	{
		using size_type       = std::size_t;
		using value_type      = T;
		using pointer         = T*;

		constexpr MemoryTrackerAllocator() noexcept = default;
		constexpr MemoryTrackerAllocator(const MemoryTrackerAllocator&) noexcept = default;
		QK_CONSTEXPR20 ~MemoryTrackerAllocator() = default;

		template <class Other>
		constexpr MemoryTrackerAllocator(const MemoryTrackerAllocator<Other>&) noexcept {}
		QK_CONSTEXPR20 MemoryTrackerAllocator& operator=(const MemoryTrackerAllocator&) = default;

		pointer allocate(size_type n)
		{
			if (auto memory = (pointer)std::malloc(n * sizeof value_type))
			{
				return memory;
			}

			throw std::bad_alloc();
		}

		void deallocate(pointer p, size_type n)
		{
			std::free(p);
		}
	};

	class DebugMemoryContext
	{
	public:
		using MemoryTracker = typename std::unordered_map<void*, std::size_t,
			std::hash<void*>,
			std::equal_to<void*>,
			MemoryTrackerAllocator<std::pair<void* const, std::size_t>>
		>;

		static DebugMemoryContext* GetContext();

	public:
		void* Allocate(std::size_t bytes);
		void* Reallocate(void* currentMemory, std::size_t bytes);

		void Deallocate(void* memory, std::size_t bytes);
		void Deallocate(void* memory);

		std::size_t GetTotalUsage() const { return m_MemoryAllocated; }
		std::size_t GetAllocatedBlocks() const { return m_Allocations - m_Deallocations; }

	private:
		void TrackAllocation(void* memory, std::size_t size);
		void TrackReallocation(void* oldMemory, void* newMemory, std::size_t size);
		void TrackDeallocation(void* memory, std::size_t size);
		void TrackDeallocation(void* memory);

	private:
		std::atomic_size_t m_Allocations     = 0;
		std::atomic_size_t m_Deallocations   = 0;
		std::atomic_size_t m_MemoryAllocated = 0;

		std::mutex m_AllocTrackerMutex;
		MemoryTracker m_MemoryTracker;
	};
}
