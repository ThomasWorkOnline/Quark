#include "qkpch.h"
#include "Quark/Core/Memory.h"

#ifdef _MSC_VER

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(std::size_t _Size)
{
    for (;;)
    {
        if (void* const block = Quark::Malloc(_Size))
        {
            return block;
        }

        if (_callnewh(_Size) == 0)
        {
            if (_Size == SIZE_MAX)
            {
                throw std::bad_array_new_length();
            }
            else
            {
                throw std::bad_alloc();
            }
        }

        // The new handler was successful; try to allocate again...
    }
}

_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(std::size_t _Size, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new(_Size);
    }
    catch (...)
    {
        return nullptr;
    }
}

#endif /* _MSC_VER */
