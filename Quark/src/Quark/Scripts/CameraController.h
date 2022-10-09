#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Input.h"

#include "Quark/Event/KeyEvent.h"
#include "Quark/Event/MouseEvent.h"

#include "Quark/Scene/NativeScriptEntity.h"

namespace Quark {

	class CameraController : public NativeScriptEntity
	{
	public:
		void OnEvent(Event& e);
		void OnUpdate(Timestep elapsedTime);

	private:
		void OnMouseScrolled(MouseScrolledEvent& e);
		void OnMouseMoved(MouseMovedEvent& e);

	private:
		Float m_ZoomSpeed = 0.0f;
		Float m_MovementSpeed = 0.0f;
		Float m_RollSensitivity = 0.0f;
		Float m_AugularMomentum = 0.0f;
	};
}
