#include "qkpch.h"
#include "Quark/Core/Memory.h"

#ifdef _MSC_VER

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](std::size_t _Size)
{
	return operator new(_Size);
}

_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](std::size_t _Size, std::nothrow_t const&) noexcept
{
	return operator new(_Size, std::nothrow);
}

#endif /* _MSC_VER */
