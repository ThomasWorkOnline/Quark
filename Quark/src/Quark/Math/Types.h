#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Quark {

/*
	#define `QK_64BIT_PRECISION` to enable double-precision physics calculations.
	This will have no effect on rendering.
*/
#ifdef QK_64BIT_PRECISION
	using Float = double;
#else
	using Float = float;
#endif

	/// Vector types
	template<uint32_t L, typename T>
	using Vec   = glm::vec<L, T>;

	using Vec2f = Vec<2, float>;
	using Vec3f = Vec<3, float>;
	using Vec4f = Vec<4, float>;

	using Vec2d = Vec<2, double>;
	using Vec3d = Vec<3, double>;
	using Vec4d = Vec<4, double>;

	using Vec2i = Vec<2, int>;
	using Vec3i = Vec<3, int>;
	using Vec4i = Vec<4, int>;

	using Vec2  = Vec<2, Float>;
	using Vec3  = Vec<3, Float>;
	using Vec4  = Vec<4, Float>;

	/// Quaternion type
	template<typename T>
	using Qua = glm::qua<T>;

	using Quatf = Qua<float>;
	using Quatd = Qua<double>;

	using Quat = Qua<Float>;

	/// Matrix types
	template<uint32_t C, uint32_t R, typename T>
	using Mat   = glm::mat<C, R, T>;

	using Mat2f = Mat<2, 2, float>;
	using Mat3f = Mat<3, 3, float>;
	using Mat4f = Mat<4, 4, float>;

	using Mat2d = Mat<2, 2, double>;
	using Mat3d = Mat<3, 3, double>;
	using Mat4d = Mat<4, 4, double>;

	using Mat2 = Mat<2, 2, Float>;
	using Mat3 = Mat<3, 3, Float>;
	using Mat4 = Mat<4, 4, Float>;
}
