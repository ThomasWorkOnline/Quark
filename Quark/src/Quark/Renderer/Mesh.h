#pragma once

#include "Quark/Core/Core.h"

#include "VertexArray.h"
#include "Buffer.h"

namespace Quark {

	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
	};

	struct IndexPack
	{
		uint32_t PositionIndex;
		uint32_t TexCoordIndex;
		uint32_t NormalIndex;

		IndexPack(uint32_t pi, uint32_t ti, uint32_t ni)
			: PositionIndex(pi), TexCoordIndex(ti), NormalIndex(ni)
		{
		}
	};

	struct OBJMeshData
	{
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec2> TexCoords;
		std::vector<glm::vec3> Normals;
		std::vector<IndexPack> FacesIndices;
		bool SmoothShaded = true;

		uint32_t VertexCount() const { return (uint32_t)FacesIndices.size(); }
	};

	struct MeshFormatDescriptor
	{
		bool ZFlip = false;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const OBJMeshData& meshData);

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }

		static Mesh LoadFromFile(std::string_view filepath, const MeshFormatDescriptor& descriptor = {});
		static Mesh ConstructMeshFromOBJData(const OBJMeshData& data);
		static Mesh GenerateUnitCube();

		// TODO: support sharp edges and sharp angle threshold detector
		static OBJMeshData ReadOBJData(std::string_view filepath, const MeshFormatDescriptor& descriptor = {});

		operator bool() const { return m_VertexArray != nullptr; }

	private:
		Ref<VertexArray> m_VertexArray;
	};
}
