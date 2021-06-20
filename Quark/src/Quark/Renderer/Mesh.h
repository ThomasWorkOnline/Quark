#pragma once

#include "../Core/Core.h"

#include "VertexArray.h"
#include "Buffer.h"

namespace Quark {

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const BufferLayout& layout, const std::string& filepath);

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }

		// TODO: support sharp edges and sharp angle threshold detector
		bool LoadOBJFromFile(const BufferLayout& layout, const std::string& filepath);

		void GenerateUnitCube();
		void GenerateTerrain(const BufferLayout& layout, size_t scale, uint32_t seed);

	private:
		Ref<VertexArray> m_VertexArray = nullptr;
	};
}
