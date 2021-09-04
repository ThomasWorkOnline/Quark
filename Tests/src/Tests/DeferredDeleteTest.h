#pragma once

#include <Quark.h>

#include <future>
#include <stack>

using namespace Quark;

class DeferredDeleteTest : public Application
{
public:
	DeferredDeleteTest()
	{
		for (uint32_t i = 0; i < 100; i++)
			m_VertexArrays.push(VertexArray::Create());
	}

	~DeferredDeleteTest()
	{
		// Testing async delete
		std::stack<std::future<void>> futures;
		while (!m_VertexArrays.empty())
		{
			futures.push(std::async(std::launch::async, DeleteAsync, m_VertexArrays.top()));
			m_VertexArrays.pop();
		}
		
		while (!futures.empty())
		{
			auto& f = futures.top();
			f.wait();
			futures.pop();
		}
	}

private:
	static void DeleteAsync(Ref<VertexArray> va)
	{
		va.reset();
	}

private:
	std::stack<Ref<VertexArray>> m_VertexArrays;
};
