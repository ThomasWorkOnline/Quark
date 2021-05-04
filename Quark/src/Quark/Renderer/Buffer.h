#pragma once

#include "../Core/Core.h"

namespace Quark {

    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static size_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:    return sizeof(float);
        case ShaderDataType::Float2:   return sizeof(float) * 2;
        case ShaderDataType::Float3:   return sizeof(float) * 3;
        case ShaderDataType::Float4:   return sizeof(float) * 4;
        case ShaderDataType::Mat3:     return sizeof(float) * 3 * 3;
        case ShaderDataType::Mat4:     return sizeof(float) * 4 * 4;
        case ShaderDataType::Int:      return sizeof(int32_t);
        case ShaderDataType::Int2:     return sizeof(int32_t) * 2;
        case ShaderDataType::Int3:     return sizeof(int32_t) * 3;
        case ShaderDataType::Int4:     return sizeof(int32_t) * 4;
        case ShaderDataType::Bool:     return sizeof(bool);
        }

        QK_FATAL("Unknown ShaderDataType");
        return 0;
    }

    struct BufferElement
    {
        std::string Name;
        ShaderDataType Type;
        size_t Size;
        size_t Offset;
        bool Normalized;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {
        }

        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
            case ShaderDataType::Float:   return 1;
            case ShaderDataType::Float2:  return 2;
            case ShaderDataType::Float3:  return 3;
            case ShaderDataType::Float4:  return 4;
            case ShaderDataType::Mat3:    return 3; // 3 * float3
            case ShaderDataType::Mat4:    return 4; // 4 * float4
            case ShaderDataType::Int:     return 1;
            case ShaderDataType::Int2:    return 2;
            case ShaderDataType::Int3:    return 3;
            case ShaderDataType::Int4:    return 4;
            case ShaderDataType::Bool:    return 1;
            }

            QK_FATAL("Unknown ShaderDataType");
            return 0;
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() = default;

        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_Elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        size_t GetStride() const { return m_Stride; }
        const std::vector<BufferElement>& GetElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    private:
        void CalculateOffsetsAndStride()
        {
            size_t offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
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

        virtual void Attach() const = 0;
        virtual void Detach() const = 0;

        virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;

        virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout& layout) = 0;

        static Ref<VertexBuffer> Create(float* vertices, size_t size);
        static Ref<VertexBuffer> Create(size_t size);
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Attach() const = 0;
        virtual void Detach() const = 0;

        virtual void SetData(uint32_t* data, uint32_t count, size_t offset = 0) = 0;

        virtual uint32_t GetCount() const = 0;

        static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
        static Ref<IndexBuffer> Create(uint32_t count);
    };
}
