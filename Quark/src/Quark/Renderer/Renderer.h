#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Scene/Components.h"

#include "Camera.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"

namespace Quark {

	class Renderer
	{
	public:
		static constexpr uint32_t FramesInFlight = 2;

		Renderer() = delete;
		Renderer& operator=(const Renderer&) = delete;

		static void BeginScene(const Camera& camera, const Transform3DComponent& cameraTransform);

		/// <summary>
		/// Receives the camera view matrix.
		/// No extra transformation is applied to the view matrix.
		/// Make sure to call `EndScene` to flush all queued objects.
		/// </summary>
		/// <param name="cameraProjection">Camera projection matrix</param>
		/// <param name="cameraView">Camera view matrix</param>
		static void BeginScene(const Mat4f& cameraProjection, const Mat4f& cameraView);
		static void EndScene();

		static ShaderLibrary& GetShaderLibrary();

	private:
		static void Initialize();
		static void Dispose();

		friend class Application;
	};
}
