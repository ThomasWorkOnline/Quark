#pragma once

#include "../Core/Core.h"
#include "../Scene/ECS/Entity.h"

namespace Entropy {

	class Camera : public Entity
	{
	public:
		Camera(float aspectRatio, float fov = 45.0f);

		void CheckAndUpdate();

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetVelocity() const { return m_Velocity; }
		const glm::quat& GetOrientation() const { return m_Orientation; }

		glm::vec3 GetFowardVector() const { return glm::vec3(0.0f, 0.0f, 1.0f) * m_Orientation; }
		glm::vec3 GetBackwardVector() const { return glm::vec3(0.0f, 0.0f, -1.0f) * m_Orientation; }
		glm::vec3 GetRightVector() const { return glm::vec3(1.0f, 0.0f, 0.0f) * m_Orientation; }
		glm::vec3 GetLeftVector() const { return glm::vec3(-1.0f, 0.0f, 0.0f) * m_Orientation; }
		glm::vec3 GetTopVector() const { return glm::vec3(0.0f, 1.0f, 0.0f) * m_Orientation; }
		glm::vec3 GetBottomVector() const { return glm::vec3(0.0f, -1.0f, 0.0f) * m_Orientation; }

		float GetFov() const { return m_Fov; }

		void SetOrientation(const glm::quat& orien) { m_Orientation = orien; m_NeedsViewUpdate = true; }
		void SetOrientation(float angle, const glm::vec3& axis) { SetOrientation(glm::angleAxis(angle, axis)); }

		void Rotate(const glm::quat& quat) { SetOrientation(m_Orientation * quat); }
		void Rotate(float angle, const glm::vec3 axis) { Rotate(glm::angleAxis(angle, axis)); }

		void SetPosition(const glm::vec3& pos) { m_Position = pos; m_NeedsViewUpdate = true; }
		void SetPosition(float x, float y, float z) { SetPosition(glm::vec3(x, y, z)); }

		void SetVelocity(const glm::vec3& vel) { m_Velocity = vel; }
		void SetVelocity(float x, float y, float z) { SetVelocity(glm::vec3(x, y, z)); }

		void SetFov(float fov) { m_Fov = fov; m_NeedsProjectionUpdate = true; }

		void SetProjectionMatrix(float aspectRatio);

	private:
		void RecalculateViewMatrix();
		void RecalculateProjectionMatrix();

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		float m_Fov;
		float m_AspectRatio;

		bool m_NeedsViewUpdate = false;
		bool m_NeedsProjectionUpdate = false;
	};
}
