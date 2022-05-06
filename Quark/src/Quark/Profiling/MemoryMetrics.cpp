#include "qkpch.h"
#include "MemoryMetrics.h"

#if QK_ENABLE_MEMORY_METRICS

void* operator new(size_t size)
{
	void* block = malloc(size);
	if (block)
	{
		::Quark::MemoryMetrics::Allocations++;
		::Quark::MemoryMetrics::MemoryAllocated += size;

		return block;
	}

	throw std::bad_alloc();
}

void operator delete(void* block, size_t size) noexcept
{
	::Quark::MemoryMetrics::Deallocations++;
	::Quark::MemoryMetrics::MemoryFreed += size;

	free(block);
}

void* operator new[](size_t size)
{
	void* block = malloc(size);
	if (block)
	{
		::Quark::MemoryMetrics::Allocations++;
		::Quark::MemoryMetrics::MemoryAllocated += size;

		return block;
	}

	throw std::bad_alloc();
}

void operator delete[](void* block, size_t size) noexcept
{
	::Quark::MemoryMetrics::Deallocations++;
	::Quark::MemoryMetrics::MemoryFreed += size;

	free(block);
}

#endif /*QK_MEMORY_METRICS*/
