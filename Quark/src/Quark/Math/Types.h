#pragma once

#define GLM_FORCE_LEFT_HANDED // <-- We use a left-handed coordinate system
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
	using Vector   = glm::vec<L, T>;

	using Vector2f = Vector<2, float>;
	using Vector3f = Vector<3, float>;
	using Vector4f = Vector<4, float>;

	using Vector2d = Vector<2, double>;
	using Vector3d = Vector<3, double>;
	using Vector4d = Vector<4, double>;

	using Vector2i = Vector<2, int>;
	using Vector3i = Vector<3, int>;
	using Vector4i = Vector<4, int>;

	using Vector2  = Vector<2, Float>;
	using Vector3  = Vector<3, Float>;
	using Vector4  = Vector<4, Float>;

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
