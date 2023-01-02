#include "qkpch.h"
#include "Buffer.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	static constexpr size_t s_ShaderDataTypeSizeLUT[] = {
		/*None                    */ 0,
		/*ShaderDataType::Float   */ sizeof(float),
		/*ShaderDataType::Float2  */ sizeof(float) * 2,
		/*ShaderDataType::Float3  */ sizeof(float) * 3,
		/*ShaderDataType::Float4  */ sizeof(float) * 4,
		/*ShaderDataType::Double  */ sizeof(double),
		/*ShaderDataType::Double2 */ sizeof(double) * 2,
		/*ShaderDataType::Double3 */ sizeof(double) * 3,
		/*ShaderDataType::Double4 */ sizeof(double) * 4,
		/*ShaderDataType::Mat3f   */ sizeof(float) * 3 * 3,
		/*ShaderDataType::Mat4f   */ sizeof(float) * 4 * 4,
		/*ShaderDataType::Mat3d   */ sizeof(double) * 3 * 3,
		/*ShaderDataType::Mat4d   */ sizeof(double) * 4 * 4,
		/*ShaderDataType::Int     */ sizeof(int32_t),
		/*ShaderDataType::Int2    */ sizeof(int32_t) * 2,
		/*ShaderDataType::Int3    */ sizeof(int32_t) * 3,
		/*ShaderDataType::Int4    */ sizeof(int32_t) * 4,
		/*ShaderDataType::Bool    */ sizeof(bool)
	};

	static constexpr uint32_t s_ShaderDataTypeComponentCountLUT[] = {
		/*None                    */ 0,
		/*ShaderDataType::Float   */ 1,
		/*ShaderDataType::Float2  */ 2,
		/*ShaderDataType::Float3  */ 3,
		/*ShaderDataType::Float4  */ 4,
		/*ShaderDataType::Double  */ 1,
		/*ShaderDataType::Double2 */ 2,
		/*ShaderDataType::Double3 */ 3,
		/*ShaderDataType::Double4 */ 4,
		/*ShaderDataType::Mat3f   */ 3,
		/*ShaderDataType::Mat4f   */ 4,
		/*ShaderDataType::Mat3d   */ 3,
		/*ShaderDataType::Mat4d   */ 4,
		/*ShaderDataType::Int     */ 1,
		/*ShaderDataType::Int2    */ 2,
		/*ShaderDataType::Int3    */ 3,
		/*ShaderDataType::Int4    */ 4,
		/*ShaderDataType::Bool    */ 1
	};

	static constexpr size_t s_IndexDataTypeSizeLUT[] = {
		/*None	 */ 0,
		/*Uint8	 */ sizeof(uint8_t),
		/*Uint16 */ sizeof(uint16_t),
		/*Uint32 */ sizeof(uint32_t)
	};

	size_t ShaderDataTypeSize(ShaderDataType type)
	{
		return s_ShaderDataTypeSizeLUT[static_cast<size_t>(type)];
	}

	size_t IndexDataTypeSize(IndexType type)
	{
		return s_IndexDataTypeSizeLUT[static_cast<size_t>(type)];
	}

	uint32_t ShaderDataTypeComponentCount(ShaderDataType type)
	{
		return s_ShaderDataTypeComponentCountLUT[static_cast<size_t>(type)];
	}

	Scope<VertexBuffer> VertexBuffer::Create(const void* vertices, size_t size)
	{
		return s_GraphicsAPI->CreateVertexBuffer(vertices, size);
	}

	Scope<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		return s_GraphicsAPI->CreateVertexBuffer(size);
	}

	IndexBuffer::IndexBuffer(uint32_t count, IndexType indexType)
		: m_Count(count), m_IndexType(indexType)
	{
	}

	Scope<IndexBuffer> IndexBuffer::Create(const void* indices, uint32_t count, IndexType indexType)
	{
		return s_GraphicsAPI->CreateIndexBuffer(indices, count, indexType);
	}

	Scope<IndexBuffer> IndexBuffer::Create(uint32_t count, IndexType indexType)
	{
		return s_GraphicsAPI->CreateIndexBuffer(count, indexType);
	}
}
