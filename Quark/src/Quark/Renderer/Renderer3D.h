#pragma once

#include "Quark/Core/Core.h"

#include "Quark/Scene/Components.h"
#include "Quark/Scene/Entity.h"
#include "Quark/Scene/Scene.h"

#include "Framebuffer.h"
#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Texture.h"

namespace Quark {

	class Renderer3D
	{
	public:
		Renderer3D() = delete;
		Renderer3D& operator=(const Renderer3D& other) = delete;

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader, const Ref<Framebuffer>& framebuffer, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
	};
}
