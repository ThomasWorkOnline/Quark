#pragma once

#include "../Core/Core.h"

#include "VertexArray.h"
#include "Buffer.h"

namespace Entropy {

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const BufferLayout& layout, const char* filepath);

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		const Ref<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }
		const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

		// TODO: support sharp edges and sharp angle threshold detector
		bool LoadOBJFromFile(const BufferLayout& layout, const char* filepath);

		void GenerateUnitCube(const BufferLayout& layout);
		void GenerateTerrain(const BufferLayout& layout, size_t scale, uint32_t seed);

	private:
		Ref<VertexArray> m_VertexArray = nullptr;
		Ref<VertexBuffer> m_VertexBuffer = nullptr;
		Ref<IndexBuffer> m_IndexBuffer = nullptr;
	};
}
