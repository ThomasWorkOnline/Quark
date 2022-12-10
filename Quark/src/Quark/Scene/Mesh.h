#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Buffer.h"

#include <vector>

namespace Quark {

	struct MeshVertex
	{
		Vec3f Position;
		Vec2f TexCoord;
		Vec3f Normal;
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
		std::vector<Vec3f> Positions;
		std::vector<Vec2f> TexCoords;
		std::vector<Vec3f> Normals;
		std::vector<IndexPack> FacesIndices;
		bool SmoothShaded = true;

		uint32_t VertexCount() const { return (uint32_t)FacesIndices.size(); }
	};

	struct StaticMesh
	{
		Scope<VertexBuffer> VertexBuffer;
		Scope<IndexBuffer>  IndexBuffer;

		StaticMesh() = default;
		StaticMesh(const OBJMeshData& meshData);

		static StaticMesh ConstructMeshFromOBJData(const OBJMeshData& data);
		static StaticMesh GenerateUnitCube();

		static const BufferLayout& GetBufferLayout();
		static OBJMeshData ReadOBJData(std::string_view filepath);

		operator bool() const { return IndexBuffer != nullptr; }
	};
}
