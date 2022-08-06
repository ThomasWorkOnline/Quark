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
		float m_ZoomSpeed = 0.0f;
		float m_MovementSpeed;
		float m_RollSensitivity;
	};
}
