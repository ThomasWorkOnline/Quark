#pragma once

#include "RenderObject.h"

#include <mutex>
#include <stack>

namespace Quark {

	class RenderObjectManager
	{
	public:
		static void DeferredDelete(RenderObject* renderObject);

		static void ReleaseRenderObjects();

	private:
		static bool HasRenderObjects();

	private:
		static std::mutex m_RenderObjectsMutex;
		static std::stack<RenderObject*> m_RenderObjects;
	};
}
