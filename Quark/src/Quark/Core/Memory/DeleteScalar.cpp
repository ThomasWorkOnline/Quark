#include "qkpch.h"
#include "Quark/Core/Memory.h"

#ifdef _MSC_VER

void __CRTDECL operator delete(void* block) noexcept
{
	Quark::Free(block);
}

void __CRTDECL operator delete(void* block, const std::nothrow_t&) noexcept
{
	operator delete(block);
}

void __CRTDECL operator delete(void* block, std::size_t size) noexcept
{
	Quark::Free(block, size);
}

void __CRTDECL operator delete(void* block, std::size_t size, const std::nothrow_t&) noexcept
{
	operator delete (block, size);
}

#endif /* _MSC_VER */
