#include "qkpch.h"
#include "MemoryMetrics.h"

namespace Quark {

	static void* Malloc(std::size_t size)
	{
		void* block = std::malloc(size);
		if (block)
		{
			MemoryMetrics::Allocations++;
			MemoryMetrics::MemoryAllocated += size;

			return block;
		}

		throw std::bad_alloc();
	}

	static void* Malloc(std::size_t size, const std::nothrow_t&)
	{
		void* block = std::malloc(size);
		if (block)
		{
			MemoryMetrics::Allocations++;
			MemoryMetrics::MemoryAllocated += size;

			return block;
		}

		return nullptr;
	}

	static void Free(void* memory, std::size_t size)
	{
		std::free(memory);

		MemoryMetrics::Deallocations++;
		MemoryMetrics::MemoryFreed += size;
	}

	static void Free(void* memory)
	{
		std::free(memory);

		MemoryMetrics::UntrackedDeallocations++;
	}
}

#ifdef QK_ENABLE_MEMORY_METRICS
#   ifdef _MSC_VER

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(
	size_t _Size
	)
{
	return Quark::Malloc(_Size);
}

_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(
	size_t _Size,
	::std::nothrow_t const&
	) noexcept
{
	return Quark::Malloc(_Size, std::nothrow);
}

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](
	size_t _Size
	)
{
	return Quark::Malloc(_Size);
}

_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](
	size_t _Size,
	::std::nothrow_t const&
	) noexcept
{
	return Quark::Malloc(_Size);
}

void __CRTDECL operator delete(
	void* _Block
	) noexcept
{
	Quark::Free(_Block);
}

void __CRTDECL operator delete(
	void* _Block,
	::std::nothrow_t const&
	) noexcept
{
	Quark::Free(_Block);
}

void __CRTDECL operator delete[](
	void* _Block
	) noexcept
{
	Quark::Free(_Block);
}

void __CRTDECL operator delete[](
	void* _Block,
	::std::nothrow_t const&
	) noexcept
{
	Quark::Free(_Block);
}

void __CRTDECL operator delete(
	void* _Block,
	size_t _Size
	) noexcept
{
	Quark::Free(_Block, _Size);
}

void __CRTDECL operator delete[](
	void* _Block,
	size_t _Size
	) noexcept
{
	Quark::Free(_Block, _Size);
}

#   endif /*_MSC_VER */
#endif /* QK_ENABLE_MEMORY_METRICS */
