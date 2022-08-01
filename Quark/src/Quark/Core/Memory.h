#pragma once

#include <memory>

// disable alloca failure warning
// since variable size stack arrays are not supported by all compilers
#pragma warning(disable : 6255)

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

	template<typename T>
	class Singleton
	{
	public:
		using ValueType = T;
		using PointerType = T*;
		using ReferenceType = T&;

		// Non-copyable
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

		// Non-movable
		Singleton(Singleton&&) = delete;
		Singleton& operator=(Singleton&&) = delete;

	public:
		static inline ReferenceType Get() { return *s_Instance; }

	protected:
		Singleton(PointerType instance) { s_Instance = instance; }
		virtual ~Singleton() { s_Instance = nullptr; }

	private:
		static inline PointerType s_Instance = nullptr;
	};
}
