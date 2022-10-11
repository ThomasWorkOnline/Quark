#pragma once

#include <Quark.h>

using namespace Quark;

class BasicRenderingApplication : public Application
{
public:
	BasicRenderingApplication(const ApplicationSpecification& spec = {})
		: Application(spec)
	{
		// Loading our texture
		m_Texture = Texture2D::Create("assets/Textures/Example1_BasicRendering.png");

		// Setting the projection type to be orthographic in screen space [-1, 1]
		m_Camera.SetOrthographic(1.0f);

		// Initial resize of the camera with the viewport size
		auto* window = GetWindow();
		m_Camera.Resize(window->GetWidth(), window->GetHeight());
	}

	virtual void OnEvent(Event& e) override
	{
		// Create an dispatcher object with the given event reference
		EventDispatcher dispatcher(e);

		// Route all `WindowResizedEvent` to `BasicRenderingApplication::OnWindowResized`
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(OnWindowResized));

		DefaultEventHandler(e);
	}

	// Called each frame
	virtual void OnRender() override
	{
		// Starting a fresh 2D scene
		Renderer2D::BeginScene(m_Camera.GetProjection(), Mat4f(1.0f)); // <-- This is the camera view matrix, we'll stick to a unit matrix

		// Submitting a unit sprite with our given texture
		Renderer2D::DrawSprite(m_Texture.get());
		//Renderer2D::DrawSprite(Vec4f(1.0f, 0.0f, 0.0f, 1.0f));

		// Telling Quark we are done with the current scene
		// The renderer will optimize and draw the geometry here
		Renderer2D::EndScene();
	}

private:
	void OnWindowResized(WindowResizedEvent& e)
	{
		// Updating the camera projection every time the window is resized
		m_Camera.Resize(e.GetWidth(), e.GetHeight());
	}

private:
	Scope<Texture2D> m_Texture;
	SceneCamera m_Camera;
};
