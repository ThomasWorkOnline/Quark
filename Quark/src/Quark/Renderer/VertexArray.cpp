#include "qkpch.h"
#include "VertexArray.h"

#include "GraphicsAPI.h"

namespace Quark {

	Ref<VertexArray> VertexArray::Create()
	{
		return GraphicsAPI::Instance->CreateVertexArray();
	}
}
