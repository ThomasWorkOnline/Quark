#pragma once

#include "Quark/Core/Core.h"

#include <limits>
#include <vector>

namespace Quark {

	enum class ShaderDataType
	{
		None = 0,

		Float,
		Float2,
		Float3,
		Float4,

		Double,
		Double2,
		Double3,
		Double4,

		Mat3f,
		Mat4f,
		Mat3d,
		Mat4d,

		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	enum class IndexType
	{
		None = 0,
		Uint8,
		Uint16,
		Uint32
	};

	size_t ShaderDataTypeSize(ShaderDataType type);
	size_t IndexDataTypeSize(IndexType type);

	uint32_t ShaderDataTypeComponentCount(ShaderDataType type);

	struct BufferElement
	{
		const char* Name;
		ShaderDataType Type;
		size_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement(ShaderDataType type, const char* name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const { return ShaderDataTypeComponentCount(Type); }
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(std::initializer_list<BufferElement> elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		size_t GetStride() const { return m_Stride; }

		uint32_t GetCount() const { return static_cast<uint32_t>(m_Elements.size()); }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		const BufferElement& operator[](std::string_view name) const
		{
			auto it = std::find_if(m_Elements.begin(), m_Elements.end(), [name](const BufferElement& element)
			{
				return element.Name == name;
			});

			QK_CORE_ASSERT(it != m_Elements.end(), "Element with name {0} was not found in layout", name);
			return *it;
		}

		const BufferElement& operator[](size_t index) const
		{
			QK_CORE_ASSERT(index >= 0 && index < m_Elements.size(), "Buffer element index out of bounds");
			return m_Elements[index];
		}

		bool operator==(const BufferLayout& other) const
		{
			if (GetCount() != other.GetCount()) return false;

			for (size_t i = 0; i < m_Elements.size(); i++)
			{
				if (m_Elements[i].Type != other.m_Elements[i].Type)
					return false;
			}

			return true;
		}

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride() noexcept
		{
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = m_Stride;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		size_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;
		virtual bool operator==(const VertexBuffer& other) const = 0;

		const BufferLayout& GetLayout() const { return m_Layout; }
		void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

		static Scope<VertexBuffer> Create(const void* vertices, size_t size);
		static Scope<VertexBuffer> Create(size_t size);

	protected:
		BufferLayout m_Layout;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t count, IndexType indexType);
		virtual ~IndexBuffer() = default;

		virtual void SetData(const uint32_t* data, uint32_t count, uint32_t firstIndex = 0) = 0;
		virtual bool operator==(const IndexBuffer& other) const = 0;

		uint32_t GetCount() const { return m_Count; }
		IndexType GetIndexType() const { return m_IndexType; }

		static Scope<IndexBuffer> Create(const uint32_t* indices, uint32_t count, IndexType indexType);
		static Scope<IndexBuffer> Create(uint32_t count, IndexType indexType);

	protected:
		uint32_t m_Count = 0;
		IndexType m_IndexType;
	};

	constexpr IndexType GetIndexTypeBasedOnIndexCount(uint32_t indexCount)
	{
		if (indexCount <= std::numeric_limits<uint8_t>::max())
		{
			return IndexType::Uint8;
		}
		else if (indexCount <= std::numeric_limits<uint16_t>::max())
		{
			return IndexType::Uint16;
		}

		return IndexType::Uint32;
	}
}
