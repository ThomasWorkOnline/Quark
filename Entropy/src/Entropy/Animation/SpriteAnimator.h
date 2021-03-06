#pragma once

#include "../Core/Core.h"

#include "../Renderer/SubTexture.h"

namespace Entropy {

	class SpriteAnimator
	{
	public:
		SpriteAnimator() = default;
		SpriteAnimator(const std::initializer_list<SubTexture2D>& textures, float animationSpeed);

		void OnUpdate(float elapsedTime);

		const SubTexture2D& GetTexture() const;

		void SetAnimationSpeed(float speed) { m_AnimationSpeed = speed; }
		float GetAnimationSpeed() const { return m_AnimationSpeed; }

	private:
		std::vector<SubTexture2D> m_Textures;
		float m_AnimationSpeed;
		
		float m_AccumulatedTime = 0.0f;
	};
}
