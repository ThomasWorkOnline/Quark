#include "qkpch.h"
#include "Quark/Core/Memory.h"

#ifdef _MSC_VER

void __CRTDECL operator delete[](void* _Block) noexcept
{
	Quark::Free(_Block);
}

void __CRTDECL operator delete[](void* _Block, const std::nothrow_t&) noexcept
{
	operator delete[](_Block);
}

void __CRTDECL operator delete[](void* _Block, std::size_t _Size) noexcept
{
	Quark::Free(_Block, _Size);
}

void __CRTDECL operator delete[](void* _Block, std::size_t _Size, const std::nothrow_t&) noexcept
{
	operator delete[](_Block, _Size);
}

#endif /* _MSC_VER */
