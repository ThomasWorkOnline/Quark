#pragma once

#include "../Core/Core.h"

#include "VertexArray.h"

namespace Entropy {

	class Mesh
	{
	public:
		Mesh();
		Mesh(const char* filepath);

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		const Ref<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }
		const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

		// TODO: support sharp edges and sharp angle threshold detector
		bool LoadOBJFromFile(const char* filepath);

		void GenerateUnitCube();
		void GenerateTerrain(size_t scale, uint32_t seed);

	private:
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
