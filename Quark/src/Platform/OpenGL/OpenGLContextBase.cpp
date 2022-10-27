#include "qkpch.h"
#include "OpenGLContextBase.h"

#include <glad/glad.h>

#if QK_ASSERT_API_VALIDATION_ERRORS
#	define QK_OPENGL_ERROR_CALLBACK(message) QK_CORE_ASSERT(false, message)
#else
#	define QK_OPENGL_ERROR_CALLBACK(message) QK_CORE_ERROR(message)
#endif

namespace Quark {

	static void OnOpenGLMessage(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         QK_OPENGL_ERROR_CALLBACK(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       QK_CORE_WARN(message);             return;
			case GL_DEBUG_SEVERITY_LOW:          QK_CORE_INFO(message);             return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: QK_CORE_TRACE(message);            return;

			QK_ASSERT_NO_DEFAULT("OnOpenGLMessage had an unknown severity level");
		}
	}

	void OpenGLContextBase::Init()
	{
		QK_PROFILE_FUNCTION();

		int success = gladLoadGL();
		QK_CORE_ASSERT(success, "Failed to initialize OpenGL context");

#ifdef QK_DEBUG
		if (GLVersion.major >= 4 && GLVersion.minor >= 3)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(OnOpenGLMessage, nullptr); // <-- This is not supported on OpenGL 4.2 or lower
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		}
#endif

		// Gamma correction
		glEnable(GL_FRAMEBUFFER_SRGB);

		// Alpha and Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Face Culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		//           ^^^-- we use a counter-clockwise winding order

		// Depth Testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Filtering
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH); // <-- NOTE: this massively slows down line rendering

		// Extensions
		//glEnable(GL_TEXTURE_2D_ARRAY_EXT);

		// Experimental
		//glEnable(GL_PROGRAM_POINT_SIZE);

		QK_CORE_TRACE("Created OpenGL graphics context!");
	}
}
