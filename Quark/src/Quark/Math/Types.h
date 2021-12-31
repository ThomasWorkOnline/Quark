#pragma once

#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Quark {

	//////////////////////////////////////////////
	/// <FloatingType> is used to perform physics calculation on bodies.
	/// `QK_DOUBLE_FLOATING_POINT` will not affect rendering behavior.
	//

#ifdef QK_DOUBLE_FLOATING_POINT
	using FloatingType = double;
#else
	using FloatingType = float;
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

#ifdef QK_DOUBLE_FLOATING_POINT
	using Vector2  = Vector<2, double>;
	using Vector3  = Vector<3, double>;
	using Vector4  = Vector<4, double>;
#else
	using Vector2 = Vector<2, float>;
	using Vector3 = Vector<3, float>;
	using Vector4 = Vector<4, float>;
#endif

	/// Quaternion type
	template<typename T>
	using Qua = glm::qua<T>;

	using Quatf = Qua<float>;
	using Quatd = Qua<double>;

#ifdef QK_DOUBLE_FLOATING_POINT
	using Quat = Qua<double>;
#else
	using Quat = Qua<float>;
#endif

	/// Matrix types
	template<uint32_t C, uint32_t R, typename T>
	using Mat   = glm::mat<C, R, T>;

	using Mat2f = Mat<2, 2, float>;
	using Mat3f = Mat<3, 3, float>;
	using Mat4f = Mat<4, 4, float>;

	using Mat2d = Mat<2, 2, double>;
	using Mat3d = Mat<3, 3, double>;
	using Mat4d = Mat<4, 4, double>;

#ifdef QK_DOUBLE_FLOATING_POINT
	using Mat2 = Mat<2, 2, double>;
	using Mat3 = Mat<3, 3, double>;
	using Mat4 = Mat<4, 4, double>;
#else
	using Mat2 = Mat<2, 2, float>;
	using Mat3 = Mat<3, 3, float>;
	using Mat4 = Mat<4, 4, float>;
#endif
}
