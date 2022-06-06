#include "qkpch.h"
#include "VertexArray.h"

#include "RenderCommand.h"

namespace Quark {

	Ref<VertexArray> VertexArray::Create()
	{
		return RenderCommand::CreateVertexArray();
	}
}
