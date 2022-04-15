#pragma once

#include "Camera.h"

namespace Quark {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera();
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);
	};
}
