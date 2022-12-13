#pragma once

#include <memory>

#if defined QK_PLATFORM_WINDOWS
#	define Freea      _freea
#	define StackAlloc _malloca
#else
	// Automatic stack cleanup
#	define Freea(mem) ((void)mem)
#	define StackAlloc __alloca
#endif

#define sizeof_array(x) (sizeof(x) / sizeof(x[0]))

namespace Quark {

	template<typename T, typename Deleter = std::default_delete<T>>
	using Scope = std::unique_ptr<T, Deleter>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename Deleter, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::shared_ptr<T>(new T(std::forward<Args>(args)...), Deleter());
	}

	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	/*
	* Holds a pointer of T allocated on the stack via alloca() or _malloca()
	* Memory is automatically freed after instance lifetime
	* NOTE: _malloca() may allocate using malloc() if the requested size is greater than the stack limit
	*		Not safe to use in time critical operations
	*/
	template<typename T>
	class AutoRelease
	{
	public:
		AutoRelease() = default;
		AutoRelease(void* memory) noexcept
			: m_Pointer(static_cast<T*>(memory))
		{
			new (m_Pointer) T{};
		}

		AutoRelease(AutoRelease&& other) noexcept
			: m_Pointer(other.m_Pointer)
		{
			other.m_Pointer = nullptr;
		}

		AutoRelease& operator=(AutoRelease&& other) noexcept
		{
			std::swap(m_Pointer, other.m_Pointer);
			return *this;
		}

		~AutoRelease() noexcept
		{
			Freea(m_Pointer);
		}

		const T* Data() const { return m_Pointer; }
		T* Data() { return m_Pointer; }

		operator const T* () const { return m_Pointer; }
		operator T* () { return m_Pointer; }

		const T& operator* () const { return *m_Pointer; }
		T& operator* () { return *m_Pointer; }

		const T& operator->() const { return *m_Pointer; }
		T& operator->() { return *m_Pointer; }

		// Non-copyable
		AutoRelease(const AutoRelease&) = delete;
		AutoRelease& operator=(const AutoRelease&) = delete;

	private:
		T* m_Pointer = nullptr;
	};

	template<typename T>
	class ArrayIterator
	{
	public:
		using value_type      = T;
		using difference_type = size_t;
		using pointer         = T*;
		using reference       = value_type&;

	public:
		ArrayIterator() = default;
		ArrayIterator(T* ptr)
			: m_Pointer(ptr)
		{}

		QK_CONSTEXPR20 bool operator==(const ArrayIterator& other) const {
			return this->m_Pointer == other.m_Pointer;
		}

		QK_CONSTEXPR20 bool operator!=(const ArrayIterator& other) const {
			return this->m_Pointer != other.m_Pointer;
		}

		QK_CONSTEXPR20 reference operator*() const noexcept { return *m_Pointer; }

		QK_CONSTEXPR20 pointer operator->() const noexcept { return m_Pointer; }

		QK_CONSTEXPR20 ArrayIterator& operator++() noexcept {
			m_Pointer++;
			return *this;
		}

		QK_CONSTEXPR20 ArrayIterator  operator++(int) noexcept {
			ArrayIterator tmp = *this;
			m_Pointer++;
			return tmp;
		}

		QK_CONSTEXPR20 ArrayIterator& operator--() noexcept {
			m_Pointer--;
			return *this;
		}

		QK_CONSTEXPR20 ArrayIterator  operator--(int) noexcept {
			ArrayIterator tmp = *this;
			m_Pointer--;
			return tmp;
		}

		QK_CONSTEXPR20 ArrayIterator& operator+=(const difference_type offset) noexcept {
			m_Pointer += offset;
			return *this;
		}

		QK_CONSTEXPR20 ArrayIterator  operator +(const difference_type offset) const noexcept {
			return (*this) + offset;
		}

		QK_CONSTEXPR20 ArrayIterator& operator-=(const difference_type offset) noexcept {
			m_Pointer -= offset;
			return *this;
		}

		QK_CONSTEXPR20 ArrayIterator  operator-(const difference_type offset) const noexcept {
			return (*this) - offset;
		}

	private:
		T* m_Pointer = nullptr;
	};

	template<typename T>
	class Array
	{
	public:
		using iterator       = ArrayIterator<T>;
		using const_iterator = const ArrayIterator<T>;

		Array() = default;
		Array(size_t elementCount)
			: m_Size(elementCount)
			, m_Memory(new T[elementCount])
		{
			auto ptr = m_Memory;
			while (ptr < m_Memory + m_Size)
			{
				new (ptr++) T{};
			}
		}

		Array(Array&& other) noexcept
			: m_Size(other.m_Size)
			, m_Memory(other.m_Memory)
		{
			other.m_Size = 0;
			other.m_Memory = nullptr;
		}

		Array& operator=(Array&& other) noexcept
		{
			m_Size = other.m_Size;
			m_Memory = other.m_Memory;
			other.m_Size = 0;
			other.m_Memory = nullptr;
			return *this;
		}

		~Array()
		{
			delete[](m_Memory);
		}

		size_t GetSize() const { return m_Size; }

		const T& operator[](size_t index) const
		{
			return m_Memory[index];
		}

		T& operator[](size_t index)
		{
			return m_Memory[index];
		}

		operator const T* () const { return m_Memory; }
		operator T* () { return m_Memory; }

		const T& operator* () const { return *m_Memory; }
		T& operator* () { return *m_Memory; }

		iterator begin() { return m_Memory; }
		iterator end() { return (m_Memory + m_Size); }

		const_iterator begin() const { return m_Memory; }
		const_iterator end() const { return (m_Memory + m_Size); }

		// Non-copyable
		Array(const Array&) = delete;
		Array& operator=(const Array&) = delete;

	private:
		size_t m_Size = 0;
		T* m_Memory = nullptr;
	};

	template<typename T>
	class Singleton
	{
	public:
		using ValueType = T;

		// Non-copyable
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

		// Non-movable
		Singleton(Singleton&&) = delete;
		Singleton& operator=(Singleton&&) = delete;

	public:
		template<typename TypeCast = ValueType>
		static inline TypeCast* Get() { return static_cast<TypeCast*>(s_Instance); }

	protected:
		Singleton(ValueType* instance) { s_Instance = instance; }
		virtual ~Singleton() { s_Instance = nullptr; }

	private:
		static inline ValueType* s_Instance = nullptr;
	};
}
