#include "qkpch.h"
#include "MemoryContext.h"

namespace Quark {

	DebugMemoryContext* DebugMemoryContext::GetContext()
	{
#ifdef QK_ENABLE_MEMORY_METRICS
		static DebugMemoryContext debugMemoryContext;
		return &debugMemoryContext;
#else
		return nullptr;
#endif
	}

	void* DebugMemoryContext::Allocate(std::size_t bytes)
	{
		void* memory = std::malloc(bytes);

		if (memory)
		{
			TrackAllocation(memory, bytes);
		}

		return memory;
	}

	void* DebugMemoryContext::Reallocate(void* currentMemory, std::size_t bytes)
	{
		void* newMemory = std::realloc(currentMemory, bytes);

		if (newMemory)
		{
			TrackReallocation(currentMemory, newMemory, bytes);
		}

		return newMemory;
	}

	void DebugMemoryContext::Deallocate(void* memory, std::size_t bytes)
	{
		TrackDeallocation(memory, bytes);
		std::free(memory);
	}

	void DebugMemoryContext::Deallocate(void* memory)
	{
		TrackDeallocation(memory);
		std::free(memory);
	}

	void DebugMemoryContext::TrackAllocation(void* memory, std::size_t size)
	{
		m_Allocations++;
		m_MemoryAllocated += size;

		std::lock_guard lock(m_AllocTrackerMutex);
		m_MemoryTracker[memory] = size;
	}

	void DebugMemoryContext::TrackReallocation(void* oldMemory, void* newMemory, std::size_t size)
	{
		std::size_t oldSize;
		{
			std::lock_guard lock(m_AllocTrackerMutex);
			QK_CORE_ASSERT_SILENT(m_MemoryTracker.find(oldMemory) != m_MemoryTracker.end(), "Cannot call realloc on a non-existing memory block");

			oldSize = m_MemoryTracker.at(oldMemory);
		}

		if (oldMemory != newMemory)
		{
			m_Allocations++;
		}

		// Only adds the added size
		m_MemoryAllocated -= oldSize;
		m_MemoryAllocated += size;
	}

	void DebugMemoryContext::TrackDeallocation(void* memory, std::size_t size)
	{
		QK_CORE_ASSERT_SILENT(!(memory == nullptr && size != 0), "Invalid deallocation, freeing a sized block at nullptr");

		{
			std::lock_guard lock(m_AllocTrackerMutex);
			QK_CORE_ASSERT_SILENT(m_MemoryTracker.find(memory) != m_MemoryTracker.end(), "Deallocation failed, block was not found");
			QK_CORE_ASSERT_SILENT(m_MemoryTracker.at(memory) == size, "Deallocation failed, size does not match");

			m_MemoryTracker.erase(memory);
		}

		m_Deallocations++;
		m_MemoryAllocated -= size;
	}

	void DebugMemoryContext::TrackDeallocation(void* memory)
	{
		std::size_t freedSize;
		{
			std::lock_guard lock(m_AllocTrackerMutex);
			QK_CORE_ASSERT_SILENT(m_MemoryTracker.find(memory) != m_MemoryTracker.end(), "Deallocation failed, block was not found");

			freedSize = m_MemoryTracker[memory];
			m_MemoryTracker.erase(memory);
		}

		m_Deallocations++;
		m_MemoryAllocated -= freedSize;
	}

#ifdef QK_ENABLE_MEMORY_METRICS

	void* Malloc(std::size_t size)
	{
		return DebugMemoryContext::GetContext()->Allocate(size);
	}

	void* Realloc(void* memory, std::size_t size)
	{
		return DebugMemoryContext::GetContext()->Reallocate(memory, size);
	}

	void Free(void* memory, std::size_t size)
	{
		DebugMemoryContext::GetContext()->Deallocate(memory, size);
	}

	void Free(void* memory)
	{
		DebugMemoryContext::GetContext()->Deallocate(memory);
	}

#else

	/////////////////////////////////
	// Return value may be nullptr
	//

	void* Malloc(std::size_t size)
	{
		return std::malloc(size);
	}

	void* Realloc(void* memory, std::size_t size)
	{
		return std::realloc(memory, size);
	}

	void Free(void* memory, std::size_t size)
	{
		std::free(memory);
	}

	void Free(void* memory)
	{
		std::free(memory);
	}

#endif /* QK_ENABLE_MEMORY_METRICS */
}
