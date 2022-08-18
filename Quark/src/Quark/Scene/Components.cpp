#include "qkpch.h"
#include "Components.h"

namespace Quark {

	Transform3DComponent::Transform3DComponent()
		: Position(0.f), Scale(1.f), Orientation(1.f, 0.f, 0.f, 0.f)
	{
	}

	Transform3DComponent::Transform3DComponent(const Transform3DComponent& other)
		: Position(other.Position), Scale(other.Scale), Orientation(other.Orientation)
	{
	}

	Transform3DComponent::Transform3DComponent(const Vec3& position, const Vec3& scale, const Quat& orientation)
		: Position(position), Scale(scale), Orientation(orientation)
	{
	}

	Vec3 Transform3DComponent::GetFrontVector() const
	{
		return Vec3(0.f, 0.f, 1.f) * Orientation;
	}

	Vec3 Transform3DComponent::GetRightVector() const
	{
		return Vec3(1.f, 0.f, 0.f) * Orientation;
	}

	Vec3 Transform3DComponent::GetTopVector() const
	{
		return Vec3(0.f, 1.f, 0.f) * Orientation;
	}

	Transform3DComponent& Transform3DComponent::SetFrontVector(const Vec3& direction)
	{
		Mat4 rotation = glm::lookAt(Position, Position + direction, { 0.f, 1.f, 0.f });
		Orientation = glm::quat_cast(rotation);
		return *this;
	}

	Transform3DComponent& Transform3DComponent::Rotate(const Quat& quat)
	{
		Orientation = Orientation * quat;
		return *this;
	}

	Transform3DComponent& Transform3DComponent::Rotate(Float angle, const Vec3& axis)
	{
		return Rotate(glm::angleAxis(angle, glm::normalize(axis)));
	}

	Mat4 Transform3DComponent::GetMatrix() const
	{
		return glm::translate(Mat4(1.f), Position) * glm::toMat4(Orientation)
			* glm::scale(Mat4(1.f), Scale);
	}

	PhysicsComponent::PhysicsComponent()
		: Velocity(0.f)
	{
	}

	PhysicsComponent::PhysicsComponent(const Vec3& velocity)
		: Velocity(velocity)
	{
	}
}
