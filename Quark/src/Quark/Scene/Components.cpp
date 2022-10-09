#include "qkpch.h"
#include "Components.h"

namespace Quark {

	TagComponent::TagComponent(std::string_view name)
		: Name(name) {}

	Vec3 Transform3DComponent::GetFrontVector() const
	{
		return Vec3(0.0, 0.0, 1.0) * Orientation;
	}

	Vec3 Transform3DComponent::GetRightVector() const
	{
		return Vec3(1.0, 0.0, 0.0) * Orientation;
	}

	Vec3 Transform3DComponent::GetTopVector() const
	{
		return Vec3(0.0, 1.0, 0.0) * Orientation;
	}

	Transform3DComponent& Transform3DComponent::SetFrontVector(const Vec3& direction)
	{
		Mat4 rotation = glm::lookAt(Position, Position + direction, { 0.0, 1.0, 0.0 });
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
		return glm::translate(Mat4(1.0), Position) * glm::toMat4(Orientation)
			* glm::scale(Mat4(1.0), Scale);
	}

	Transform3DComponent::Transform3DComponent()
		: Position(0.0), Scale(1.0), Orientation(1.0, 0.0, 0.0, 0.0)
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

	PhysicsComponent::PhysicsComponent()
		: Velocity(0.0)
	{
	}

	PhysicsComponent::PhysicsComponent(const Vec3& velocity)
		: Velocity(velocity)
	{
	}

	void PhysicsComponent::AddForce(const Vec3& force)
	{
		Velocity += force;
	}
}
