#pragma once

#include "../Core/Core.h"

#include "VertexArray.h"
#include "Buffer.h"
#include "CubeMap.h"

namespace Entropy {

	class Environment
	{
	public:
		Environment(const std::array<std::string, 6>& filepaths);
		Environment(const char* directory);

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		const Ref<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }
		const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

		const Ref<CubeMap>& GetCubeMap() const { return m_CubeMap; }

	private:
		Ref<CubeMap> m_CubeMap;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
