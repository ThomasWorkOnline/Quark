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

#define MATERIAL 0

	{
		TextureDescriptor descriptor;
		descriptor.SRGB = true;
		descriptor.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		descriptor.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

#if MATERIAL == 0
		m_Albedo           = Texture2D::Create("assets/textures/pbr/greasy-pan/greasy-pan-2-albedo.png", descriptor);
		m_Metallic         = Texture2D::Create("assets/textures/pbr/greasy-pan/greasy-pan-2-metal.png");
		m_Normal           = Texture2D::Create("assets/textures/pbr/greasy-pan/greasy-pan-2-normal.png");
		m_Roughness        = Texture2D::Create("assets/textures/pbr/greasy-pan/greasy-pan-2-roughness.png");
#endif

#if MATERIAL == 1
		m_Albedo           = Texture2D::Create("assets/textures/pbr/copper-scuffed/Copper-scuffed_basecolor.png", descriptor);
		m_Metallic         = Texture2D::Create("assets/textures/pbr/copper-scuffed/Copper-scuffed_metallic.png");
		m_Normal           = Texture2D::Create("assets/textures/pbr/copper-scuffed/Copper-scuffed_normal.png");
		m_Roughness        = Texture2D::Create("assets/textures/pbr/copper-scuffed/Copper-scuffed_roughness.png");
#endif

#if MATERIAL == 2
		m_Albedo           = Texture2D::Create("assets/textures/pbr/streaked-metal/albedo.png", descriptor);
		m_Metallic         = Texture2D::Create("assets/textures/pbr/streaked-metal/metalness.png");
		m_Normal           = Texture2D::Create("assets/textures/pbr/streaked-metal/normal-dx.png");
		m_Roughness        = Texture2D::Create("assets/textures/pbr/streaked-metal/rough.png");
#endif

#if MATERIAL == 3
		m_Albedo           = Texture2D::Create("assets/textures/pbr/rustediron/rustediron2_basecolor.png", descriptor);
		m_Metallic         = Texture2D::Create("assets/textures/pbr/rustediron/rustediron2_metallic.png");
		m_Normal           = Texture2D::Create("assets/textures/pbr/rustediron/rustediron2_normal.png");
		m_Roughness        = Texture2D::Create("assets/textures/pbr/rustediron/rustediron2_roughness.png");
#endif
	}

	m_HDRMap = Texture2D::Create("assets/textures/hdr/MonValley_G_DirtRoad_3k.hdr");
	m_HDRCubeMap = Cubemap::Create(512, 512);

	m_PBRShader = Shader::Create("assets/shaders/PBR.glsl");
	m_IrradianceShader = Shader::Create("assets/shaders/irradiance.glsl");
	m_EquirectangleToCubemap = Shader::Create("assets/shaders/equirectangleToCubemap.glsl");

	m_PBRShader->Attach();
	m_PBRShader->SetMat4("u_Model", glm::mat4(1.0f));
	m_PBRShader->SetInt("u_AlbedoMap",           0);
	m_PBRShader->SetInt("u_NormalMap",           1);
	m_PBRShader->SetInt("u_MetallicMap",         2);
	m_PBRShader->SetInt("u_RoughnessMap",        3);
	m_PBRShader->SetInt("u_IrradianceMap",       4);

	static constexpr float lightPower = 50.0f;
	static constexpr glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f) * lightPower;
	static constexpr glm::vec3 lightPositions[4] = {
		{  0.0f,  0.0f, -3.0f },
		{  0.0f,  2.0f,  3.0f },
		{  1.7f,  0.3f, -0.5f },
		{  2.0f,  2.0f, -2.0f }
	};

	m_PBRShader->SetFloat3("u_LightColors[0]", lightColor);
	m_PBRShader->SetFloat3("u_LightColors[1]", lightColor);
	m_PBRShader->SetFloat3("u_LightColors[2]", lightColor);
	m_PBRShader->SetFloat3("u_LightColors[3]", lightColor);

	m_PBRShader->SetFloat3("u_LightPositions[0]", lightPositions[0]);
	m_PBRShader->SetFloat3("u_LightPositions[1]", lightPositions[1]);
	m_PBRShader->SetFloat3("u_LightPositions[2]", lightPositions[2]);
	m_PBRShader->SetFloat3("u_LightPositions[3]", lightPositions[3]);

	{
		FramebufferAttachmentSpecification colorAttachmentSpec;
		colorAttachmentSpec.Attachment = FramebufferAttachment::ColorAttachment0;
		colorAttachmentSpec.InternalFormat = TextureInternalFormat::RGB16f;

		FramebufferSpecification spec;
		spec.Width = 512;
		spec.Height = 512;
		spec.Attachments = { colorAttachmentSpec };
		m_IrradianceFramebuffer = Framebuffer::Create(spec);
	}

	m_IrradianceFramebuffer->Attach();
	m_IrradianceFramebuffer->Detach();

	// Due to the lack of a render pipeline, we must reset the viewport after detaching the fbo
	auto& window = GetWindow();
	RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
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
	// TODO: attach irradiance map

	m_PBRShader->Attach();
	m_PBRShader->SetFloat3("u_CameraPos", transform.Position);

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
