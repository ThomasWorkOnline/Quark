#pragma once

#include <type_traits>

namespace Quark {

	// SFINAE Test
	template <typename T>
	class HasOnCreate
	{
	private:
		static constexpr std::true_type  Test(decltype(&T::OnCreate));
		static constexpr std::false_type Test(...);

	public:
		static constexpr bool value = decltype(Test(0))::value;
	};

	template <typename T>
	class HasOnDestroy
	{
	private:
		static constexpr std::true_type  Test(decltype(&T::OnDestroy));
		static constexpr std::false_type Test(...);

	public:
		static constexpr bool value = decltype(Test(0))::value;
	};

	template <typename T>
	class HasOnUpdate
	{
	private:
		static constexpr std::true_type  Test(decltype(&T::OnUpdate));
		static constexpr std::false_type Test(...);

	public:
		static constexpr bool value = decltype(Test(0))::value;
	};

	template <typename T>
	class HasOnEvent
	{
	private:
		static constexpr std::true_type  Test(decltype(&T::OnEvent));
		static constexpr std::false_type Test(...);

	public:
		static constexpr bool value = decltype(Test(0))::value;
	};
}
