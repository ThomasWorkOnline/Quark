#pragma once

#include <memory>
#include <stdexcept>

#if defined QK_PLATFORM_WINDOWS
#	define Freea      _freea
#	define StackAlloc _malloca
#else
	// Automatic stack cleanup
#	define Freea
#	define StackAlloc alloca
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

	extern void* Malloc(std::size_t size);
	extern void* Realloc(void* memory, std::size_t size);

	extern void  Free(void* memory, std::size_t size);
	extern void  Free(void* memory);

	template<typename T>
	class AutoRelease
	{
	public:
		AutoRelease(void* memory) noexcept
			: m_Pointer(static_cast<T*>(memory))
		{}

		~AutoRelease() noexcept
		{
			Freea(m_Pointer);
		}

		operator T* () const { return m_Pointer; }

		const T& operator* () const { return *m_Pointer; }
		T& operator* () { return *m_Pointer; }

		const T& operator->() const { return *m_Pointer; }
		T& operator->() { return *m_Pointer; }

	private:
		T* m_Pointer = nullptr;
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
