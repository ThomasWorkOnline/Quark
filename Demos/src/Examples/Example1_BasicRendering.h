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
		auto* window = GetWindow();
		m_Camera.SetOrthographic(window->GetAspectRatio(), 1.0f);
	}

	virtual void OnEvent(Event& e) override
	{
		// Create an dispatcher object with the given event reference
		EventDispatcher dispatcher(e);

		// Route all `WindowResizedEvent` to `BasicRenderingApplication::OnWindowResized`
		dispatcher.Dispatch<ViewportResizedEvent>(ATTACH_EVENT_FN(OnViewportResized));

		DefaultEventHandler(e);
	}

	// Called each frame
	virtual void OnRender() override
	{
		// Starting a fresh 2D scene
		Renderer2D::BeginScene(m_Camera.GetProjection());

		// Submitting a unit sprite with our given texture
		Renderer2D::DrawSprite(m_Texture.get());
		//Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f, 1.0f });

		// Telling Quark we are done with the current scene
		// The renderer will optimize and draw the geometry here
		Renderer2D::EndScene();
	}

private:
	void OnViewportResized(ViewportResizedEvent& e)
	{
		// Updating the camera projection every time the viewport is resized
		m_Camera.Resize(e.GetAspectRatio(), 1.0f);
	}

private:
	Scope<Texture2D> m_Texture;
	SceneCamera m_Camera;
};
