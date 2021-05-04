#pragma once

#include "../Core/Core.h"

#include "../Renderer/SubTexture.h"

namespace Quark {

	class SpriteAnimator
	{
	public:
		SpriteAnimator() = default;
		SpriteAnimator(const std::initializer_list<SubTexture2D>& textures, float animationSpeed);

		void OnUpdate(float elapsedTime);

		const SubTexture2D& GetTexture() const;

		void Pause() { m_Paused = true; }
		void Resume() { m_Paused = false; }

		void SetAnimationSpeed(float speed) { m_AnimationSpeed = speed; }
		float GetAnimationSpeed() const { return m_AnimationSpeed; }

	private:
		std::vector<SubTexture2D> m_Textures;
		float m_AnimationSpeed;
		float m_AccumulatedTime = 0.0f;
		bool m_Paused = false;
	};
}
