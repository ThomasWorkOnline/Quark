#include "PBRRendering.h"

PBRRendering::PBRRendering()
{
	m_Player = m_Scene.CreateEntity();
	m_Player.AddComponent<Transform3DComponent>().Position = { 0.0f, 0.0f, -2.0f };
	m_Player.AddComponent<PhysicsComponent>().Friction = 4.0f;
	m_Player.AddComponent<PerspectiveCameraComponent>((float)GetWindow().GetWidth() / GetWindow().GetHeight(), 70.0f);
	m_Controller = { m_Player };

	{
#if 1
		MeshDescriptor descriptor;
		descriptor.ZFlip = false;
		m_Body.LoadOBJFromFile("assets/meshes/poly_sphere.obj", descriptor);
#else
		m_Body.GenerateUnitCube();
#endif
	}

	{
		TextureDescriptor descriptor;
		descriptor.SRGB = true;
		descriptor.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		descriptor.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Albedo           = Texture2D::Create("assets/textures/pbr/streaked-metal/albedo.png", descriptor);
		m_Normal           = Texture2D::Create("assets/textures/pbr/streaked-metal/normal-dx.png", descriptor);
		m_Metallic         = Texture2D::Create("assets/textures/pbr/streaked-metal/metalness.png");
		m_Roughness        = Texture2D::Create("assets/textures/pbr/streaked-metal/rough.png");
		m_AmbiantOcclusion = Texture2D::Create("assets/textures/pbr/streaked-metal/ao.png", descriptor);
	}

	m_HDRMap = Texture2D::Create("assets/textures/hdr/MonValley_G_DirtRoad_3k.hdr");

	m_Shader = Shader::Create("assets/shaders/PBR.glsl");
	m_Shader->Attach();
	m_Shader->SetMat4("u_Model", glm::mat4(1.0f));
	m_Shader->SetInt("u_AlbedoMap",           0);
	m_Shader->SetInt("u_NormalMap",           1);
	m_Shader->SetInt("u_MetallicMap",         2);
	m_Shader->SetInt("u_RoughnessMap",        3);
	m_Shader->SetInt("u_AmbiantOcclusionMap", 4);

	static constexpr float lightPower = 10.0f;
	static constexpr glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f) * lightPower;
	static constexpr glm::vec3 lightPositions[4] = {
		{  0.0f,  0.0f, -3.0f },
		{  0.0f,  1.0f,  3.0f },
		{  1.7f,  0.3f, -0.5f },
		{  2.0f,  2.0f, -1.0f }
	};

	m_Shader->SetFloat3("u_LightColors[0]", lightColor);
	m_Shader->SetFloat3("u_LightColors[1]", lightColor);
	m_Shader->SetFloat3("u_LightColors[2]", lightColor);
	m_Shader->SetFloat3("u_LightColors[3]", lightColor);

	m_Shader->SetFloat3("u_LightPositions[0]", lightPositions[0]);
	m_Shader->SetFloat3("u_LightPositions[1]", lightPositions[1]);
	m_Shader->SetFloat3("u_LightPositions[2]", lightPositions[2]);
	m_Shader->SetFloat3("u_LightPositions[3]", lightPositions[3]);
}

void PBRRendering::OnUpdate(Timestep elapsedTime)
{
	m_Controller.OnUpdate(elapsedTime);
	m_Scene.OnUpdate(elapsedTime);
	
	const auto& camera = m_Player.GetComponent<PerspectiveCameraComponent>().Camera;
	const auto& transform = m_Player.GetComponent<Transform3DComponent>();

	Renderer::BeginScene(camera, transform);

	m_Albedo->Attach(0);
	m_Normal->Attach(1);
	m_Metallic->Attach(2);
	m_Roughness->Attach(3);
	m_AmbiantOcclusion->Attach(4);

	m_Shader->Attach();
	m_Shader->SetFloat3("u_CameraPos", transform.Position);

	RenderCommand::DrawIndexed(m_Body.GetVertexArray());

	Renderer::EndScene();
}

void PBRRendering::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(PBRRendering::OnKeyPressed));
	dispatcher.Dispatch<MouseMovedEvent>(ATTACH_EVENT_FN(PBRRendering::OnMouseMoved));
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(PBRRendering::OnMouseButtonPressed));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(ATTACH_EVENT_FN(PBRRendering::OnMouseButtonReleased));

	e.Handled = e.IsInCategory(EventCategoryInput) && GetWindow().IsCursorEnabled();

	if (!e.Handled)
		m_Controller.OnEvent(e);
}

bool PBRRendering::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::Escape:
			auto& window = GetWindow();
			window.EnableCursor();
			break;
	}
	return false;
}

bool PBRRendering::OnMouseMoved(MouseMovedEvent& e)
{
	return !GetWindow().IsCursorEnabled();
}

bool PBRRendering::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	GetWindow().DisableCursor();
	return false;
}

bool PBRRendering::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	return false;
}
