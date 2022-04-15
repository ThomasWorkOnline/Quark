#pragma once

#include "Quark/Core/Core.h"

#include "Quark/Scene/Components.h"
#include "Quark/Scene/Entity.h"
#include "Quark/Scene/Scene.h"

#include "Renderer2D.h"

#include "Font.h"
#include "Framebuffer.h"
#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Texture.h"

namespace Quark {

	class Renderer
	{
	public:
		Renderer() = delete;
		Renderer& operator=(const Renderer& other) = delete;

		static void Initialize();
		static void Dispose();

		static void BeginScene(const Camera& sceneCamera, const Transform3DComponent& cameraTransform);

		/// <summary>
		/// Receives the camera view matrix.
		/// No extra transformation is applied to the view matrix.
		/// Make sure to call `EndScene` to flush all queued objects.
		/// </summary>
		/// <param name="cameraProjection">Camera projection matrix</param>
		/// <param name="cameraView">Camera view matrix</param>
		static void BeginScene(const glm::mat4& cameraProjection, const glm::mat4& cameraView);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader, const Ref<Framebuffer>& framebuffer, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));

		static void OnWindowResized(uint32_t width, uint32_t height);

		static ShaderLibrary& GetShaderLibrary() { return s_ShaderLibrary; }

	private:
		static ShaderLibrary s_ShaderLibrary;
	};
}
