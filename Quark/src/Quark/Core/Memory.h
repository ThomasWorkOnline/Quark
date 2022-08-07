#pragma once

#include <memory>

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
