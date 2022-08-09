#include "qkpch.h"
#include "Core.h"

namespace Quark {

	namespace RuntimeCore {

		static size_t s_MemAllocatedAfterStaticInit = 0;

		void Init()
		{
#ifdef QK_ENABLE_MEMORY_METRICS
			(void)Logger::GetInstance();
			s_MemAllocatedAfterStaticInit = DebugMemoryContext::GetContext()->GetTotalUsage();
#endif
		}

		void Shutdown()
		{
#ifdef QK_ENABLE_MEMORY_METRICS
			size_t memAllocatedAfterAppCleanup = DebugMemoryContext::GetContext()->GetTotalUsage();
			size_t blocksAllocated = DebugMemoryContext::GetContext()->GetAllocatedBlocks();

			size_t bytesLeaked = memAllocatedAfterAppCleanup - s_MemAllocatedAfterStaticInit;
			if (bytesLeaked > 0)
			{
				QK_CORE_WARN("Application memory leak detected: {0} bytes lost, {1} blocks not deleted\t"
					"(this is likely due to static initialization. It will get cleaned up after main() returns)", bytesLeaked, blocksAllocated);
			}
#endif
		}
	}
}
