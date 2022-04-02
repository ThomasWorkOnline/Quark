#include "MemoryMetrics.h"

#ifdef QK_MEMORY_METRICS

void* operator new(size_t size)
{
	void* block = malloc(size);
	if (block)
	{
		using Metrics = ::Quark::MemoryMetrics;
		Metrics::Allocations++;
		Metrics::MemoryAllocated += size;

		return block;
	}

	throw std::bad_alloc();
}

void operator delete(void* block, size_t size) noexcept
{
	using Metrics = ::Quark::MemoryMetrics;
	Metrics::Deallocations++;
	Metrics::MemoryFreed += size;

	free(block);
}

void* operator new[](size_t size)
{
	void* block = malloc(size);
	if (block)
	{
		using Metrics = ::Quark::MemoryMetrics;
		Metrics::Allocations++;
		Metrics::MemoryAllocated += size;

		return block;
	}

	throw std::bad_alloc();
}

void operator delete[](void* block, size_t size) noexcept
{
	using Metrics = ::Quark::MemoryMetrics;
	Metrics::Deallocations++;
	Metrics::MemoryFreed += size;

	free(block);
}

#endif /*QK_MEMORY_METRICS*/
