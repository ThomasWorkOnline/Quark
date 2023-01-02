#pragma once

#include "Quark/Core/Core.h"

#ifdef QK_PLATFORM_APPLE
	#include <MoltenVK/mvk_vulkan.h>
#else
	#include <vulkan/vulkan.h>
#endif

#include <vma/vk_mem_alloc.h>

#if defined(QK_DEBUG)
	#define QK_ENABLE_VULKAN_VALIDATION_LAYERS 1
#else
	#define QK_ENABLE_VULKAN_VALIDATION_LAYERS 0
#endif
