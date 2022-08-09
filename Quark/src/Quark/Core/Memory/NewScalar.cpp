#include "qkpch.h"
#include "Quark/Core/Memory.h"

#ifdef _MSC_VER

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(std::size_t _Size)
{
	if (void* memory = Quark::Malloc(_Size))
	{
		return memory;
	}

	throw std::bad_alloc();
}

_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(std::size_t _Size, const std::nothrow_t&) noexcept
{
	return Quark::Malloc(_Size);
}

#endif /* _MSC_VER */
