#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Input.h"

#include "Quark/Event/KeyEvent.h"
#include "Quark/Event/MouseEvent.h"

#include "Quark/Scene/Components.h"
#include "Quark/Scene/NativeScriptEntity.h"

namespace Quark {

	class CameraController : public NativeScriptEntity
	{
	public:
		virtual void OnUpdate(Timestep elapsedTime) override;
		virtual void OnEvent(Event& e) override;

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);

	private:
		float m_RollSensitivity = 1.0f;
		float m_MovementSpeed = 12.0f;
		float m_MouseSensitivity = 0.002f;
		float m_MouseScrollSensitivity = 1.0f;

		float m_ZoomSpeed = 0.0f;
		float m_DefaultMovementSpeed = m_MovementSpeed;
		float m_DefaultRollSensitivity = m_RollSensitivity;
	};
}
