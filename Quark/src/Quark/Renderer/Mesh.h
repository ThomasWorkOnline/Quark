#pragma once

#include "Quark/Core/Core.h"

#include "VertexArray.h"
#include "Buffer.h"

namespace Quark {

	struct MeshDescriptor
	{
		bool ZFlip = false;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(std::string_view filepath, const MeshDescriptor& descriptor = {});

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }

		// TODO: support sharp edges and sharp angle threshold detector
		void LoadOBJFromFile(std::string_view filepath, const MeshDescriptor& descriptor = {});
		void GenerateUnitCube();

	private:
		Ref<VertexArray> m_VertexArray = nullptr;
	};
}
