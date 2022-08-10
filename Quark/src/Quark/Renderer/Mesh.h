#pragma once

#include "Quark/Core/Core.h"

#include "VertexArray.h"
#include "Buffer.h"

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

	struct MeshFormatDescriptor
	{
		bool ZFlip = false;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const OBJMeshData& meshData);

		VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
		IndexBuffer*  GetIndexBuffer() const { return m_IndexBuffer.get(); }

		static Mesh LoadFromFile(std::string_view filepath, const MeshFormatDescriptor& descriptor = {});
		static Mesh ConstructMeshFromOBJData(const OBJMeshData& data);
		static Mesh GenerateUnitCube();

		// TODO: support sharp edges and sharp angle threshold detector
		static OBJMeshData ReadOBJData(std::string_view filepath, const MeshFormatDescriptor& descriptor = {});

		operator bool() const { return m_VertexBuffer != nullptr; }

	private:
		Scope<VertexBuffer> m_VertexBuffer;
		Scope<IndexBuffer> m_IndexBuffer;
	};
}
