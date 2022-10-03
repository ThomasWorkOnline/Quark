#pragma once

#include <type_traits>

namespace Quark {

	// SFINAE Test
	template <typename T>
	class HasOnCreate
	{
	private:
		template <typename C>
		static constexpr std::true_type Test(decltype(&C::OnCreate));

		template <typename C>
		static constexpr std::false_type Test(...);

	public:
		static constexpr bool value = std::is_same_v<decltype(Test<T>(0)), std::true_type>;
	};

	template <typename T>
	class HasOnDestroy
	{
	private:
		template <typename C>
		static constexpr std::true_type Test(decltype(&C::OnDestroy));

		template <typename C>
		static constexpr std::false_type Test(...);

	public:
		static constexpr bool value = std::is_same_v<decltype(Test<T>(0)), std::true_type>;
	};

	template <typename T>
	class HasOnUpdate
	{
	private:
		template <typename C>
		static constexpr std::true_type Test(decltype(&C::OnUpdate));

		template <typename C>
		static constexpr std::false_type Test(...);

	public:
		static constexpr bool value = std::is_same_v<decltype(Test<T>(0)), std::true_type>;
	};

	template <typename T>
	class HasOnEvent
	{
	private:
		template <typename C>
		static constexpr std::true_type Test(decltype(&C::OnEvent));

		template <typename C>
		static constexpr std::false_type Test(...);

	public:
		static constexpr bool value = std::is_same_v<decltype(Test<T>(0)), std::true_type>;
	};
}
