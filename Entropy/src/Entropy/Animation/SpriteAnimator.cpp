#include "SpriteAnimator.h"

namespace Entropy {

	SpriteAnimator::SpriteAnimator(const std::initializer_list<SubTexture2D>& textures, float animationSpeed)
		: m_Textures(textures), m_AnimationSpeed(animationSpeed)
	{
	}

	void SpriteAnimator::OnUpdate(float elapsedTime)
	{
		m_AccumulatedTime += elapsedTime;
	}

	const SubTexture2D& SpriteAnimator::GetTexture() const
	{
		uint32_t textureIndex = (uint32_t)(m_AccumulatedTime * m_AnimationSpeed) % m_Textures.size();
		return m_Textures[textureIndex];
	}
}
