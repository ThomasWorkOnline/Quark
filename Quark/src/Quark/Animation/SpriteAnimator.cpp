#include "SpriteAnimator.h"

namespace Quark {

	SpriteAnimator::SpriteAnimator(std::initializer_list<SubTexture2D> textures, float animationSpeed)
		: m_Textures(textures), m_AnimationSpeed(animationSpeed)
	{
	}

	void SpriteAnimator::OnUpdate(float elapsedTime)
	{
		if (!m_Paused)
			m_AccumulatedTime += elapsedTime;
	}

	const SubTexture2D& SpriteAnimator::GetTexture() const
	{
		uint32_t textureIndex = (uint32_t)(m_AccumulatedTime * m_AnimationSpeed) % (uint32_t)m_Textures.size();
		return m_Textures[textureIndex];
	}
}
