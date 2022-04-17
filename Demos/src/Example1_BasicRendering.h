#pragma once

#include <Quark.h>

class YourApplication : public Quark::Application
{
public:
	YourApplication()
	{
		// Loading our texture
		m_Texture = Quark::Texture2D::Create("assets/textures/Example1_BasicRendering.png");

		auto& window = GetWindow();
		float aspectRatio = (float)window.GetWidth() / window.GetHeight();

		// Updating the projection with the screen's aspect ratio
		m_Camera.SetProjection(-aspectRatio, aspectRatio, -1, 1);
	}

	void OnEvent(Quark::Event& e) override
	{
		// Create an dispatcher object with the given event reference
		Quark::EventDispatcher dispatcher(e);

		// Route all `WindowResizedEvent` to `YourApplication::OnWindowResized`
		dispatcher.Dispatch<Quark::WindowResizedEvent>(ATTACH_EVENT_FN(YourApplication::OnWindowResized));
	}

	// Called each frame
	// Elapsed time holds the time delta in seconds between frames
	void OnUpdate(Quark::Timestep elapsedTime) override
	{
		// Starting a fresh scene
		Quark::Renderer::BeginScene(m_Camera.GetProjection(), glm::mat4(1.0f)); // < --This is the camera view matrix, we'll stick to a unit matrix

		// Submitting a unit sprite with our given texture
		Quark::Renderer2D::DrawSprite(m_Texture);

		// Telling Quark we are done with the current scene
		// The renderer will optimize and draw the geometry here
		Quark::Renderer::EndScene();
	}

private:
	bool OnWindowResized(Quark::WindowResizedEvent& e)
	{
		// Updating the camera projection every time the window is resized
		float aspectRatio = (float)e.GetWidth() / e.GetHeight();
		m_Camera.SetProjection(-aspectRatio, aspectRatio, -1, 1);
		return false;
	}

private:
	Quark::Ref<Quark::Texture2D> m_Texture;
	Quark::OrthographicCamera m_Camera;
};
