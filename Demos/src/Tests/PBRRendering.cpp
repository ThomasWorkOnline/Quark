#include "PBRRendering.h"

PBRRendering::PBRRendering()
{
	m_Player = m_Scene.CreateEntity();
	m_Player.AddComponent<Transform3DComponent>().Position = { 0.0f, 0.0f, -2.0f };
	m_Player.AddComponent<PhysicsComponent>().Friction = 4.0f;
	m_Player.AddComponent<PerspectiveCameraComponent>((float)GetWindow().GetWidth() / GetWindow().GetHeight(), 70.0f);
	m_Controller = { m_Player };

#if 0
	m_Body.LoadOBJFromFile("assets/meshes/poly_sphere.obj");
#else
	m_Body.GenerateUnitCube();
#endif

	m_Cube.GenerateUnitCube();
	m_CubemapVAO = m_Cube.GetVertexArray();

#define MATERIAL 2

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
		m_Albedo           = Texture2D::Create("assets/textures/pbr/streaked-metal/albedo.png", descriptor);
		m_Metallic         = Texture2D::Create("assets/textures/pbr/streaked-metal/metalness.png");
		m_Normal           = Texture2D::Create("assets/textures/pbr/streaked-metal/normal-dx.png");
		m_Roughness        = Texture2D::Create("assets/textures/pbr/streaked-metal/rough.png");
#endif

#if MATERIAL == 2
		m_Albedo           = Texture2D::Create("assets/textures/pbr/rustediron/rustediron2_basecolor.png", descriptor);
		m_Metallic         = Texture2D::Create("assets/textures/pbr/rustediron/rustediron2_metallic.png");
		m_Normal           = Texture2D::Create("assets/textures/pbr/rustediron/rustediron2_normal.png");
		m_Roughness        = Texture2D::Create("assets/textures/pbr/rustediron/rustediron2_roughness.png");
#endif
	}

	m_HDRTexture = Texture2D::Create("assets/textures/hdr/MonValley_G_DirtRoad_3k.hdr");

	m_PBRShader = Shader::Create("assets/shaders/PBR.glsl");
	m_IrradianceShader = Shader::Create("assets/shaders/irradiance.glsl");
	m_EquirectangleToCubemapShader = Shader::Create("assets/shaders/equirectangleToCubemap.glsl");
	m_SkyboxShader = Shader::Create("assets/shaders/cubemap.glsl");

	{
		FramebufferSpecification spec;
		spec.Width = 2048;
		spec.Height = 2048;

		m_EnvironmentFramebuffer = Framebuffer::Create(spec);
	}

	static constexpr float lightPower = 10.0f;
	static constexpr glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f) * lightPower;
	static constexpr glm::vec3 lightPositions[4] = {
		{  0.0f,  0.0f, -3.0f },
		{  0.0f,  2.0f,  3.0f },
		{ -1.7f,  0.3f, -0.5f },
		{ -2.0f,  2.0f,  2.0f }
	};

	m_PBRShader->Attach();
	m_PBRShader->SetMat4("u_Model", glm::mat4(1.0f));
	m_PBRShader->SetInt("u_AlbedoMap", 0);
	m_PBRShader->SetInt("u_NormalMap", 1);
	m_PBRShader->SetInt("u_MetallicMap", 2);
	m_PBRShader->SetInt("u_RoughnessMap", 3);
	m_PBRShader->SetInt("u_IrradianceMap", 4);

	m_PBRShader->SetFloat3("u_LightColors[0]", lightColor);
	m_PBRShader->SetFloat3("u_LightColors[1]", lightColor);
	m_PBRShader->SetFloat3("u_LightColors[2]", lightColor);
	m_PBRShader->SetFloat3("u_LightColors[3]", lightColor);

	m_PBRShader->SetFloat3("u_LightPositions[0]", lightPositions[0]);
	m_PBRShader->SetFloat3("u_LightPositions[1]", lightPositions[1]);
	m_PBRShader->SetFloat3("u_LightPositions[2]", lightPositions[2]);
	m_PBRShader->SetFloat3("u_LightPositions[3]", lightPositions[3]);

	m_SkyboxShader->Attach();
	m_SkyboxShader->SetInt("u_EnvironmentMap", 0);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] = {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	m_HDRCubemap = Cubemap::Create(2048, 2048);
	m_Irradiance = Cubemap::Create(32, 32);

	m_EnvironmentFramebuffer->Attach();

	m_EquirectangleToCubemapShader->Attach();
	m_EquirectangleToCubemapShader->SetInt("u_EquirectangularMap", 0);
	m_EquirectangleToCubemapShader->SetMat4("u_Projection", captureProjection);
	m_HDRTexture->Attach(0);

	RenderCommand::SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });
	RenderCommand::SetCullFace(RenderCullFace::Front);
	RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

	for (uint8_t i = 0; i < 6; i++)
	{
		m_EquirectangleToCubemapShader->SetMat4("u_View", captureViews[i]);
		m_EnvironmentFramebuffer->AttachColorTextureTarget(0x8515 + i, m_HDRCubemap->GetRendererID());

		RenderCommand::Clear();
		RenderCommand::DrawIndexed(m_CubemapVAO);
	}

	m_EnvironmentFramebuffer->Detach();
	m_EnvironmentFramebuffer->Resize(32, 32);
	m_EnvironmentFramebuffer->Attach();

	m_IrradianceShader->Attach();
	m_IrradianceShader->SetInt("u_EnvironmentMap", 0);
	m_IrradianceShader->SetMat4("u_Projection", captureProjection);
	m_HDRCubemap->Attach(0);

	for (uint8_t i = 0; i < 6; i++)
	{
		m_IrradianceShader->SetMat4("u_View", captureViews[i]);
		m_EnvironmentFramebuffer->AttachColorTextureTarget(0x8515 + i, m_Irradiance->GetRendererID());

		RenderCommand::Clear();
		RenderCommand::DrawIndexed(m_CubemapVAO);
	}

	m_EnvironmentFramebuffer->Detach();

	// Due to the lack of a render pipeline, we must reset the viewport after detaching the fbo
	auto& window = GetWindow();
	RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
	RenderCommand::SetCullFace(RenderCullFace::Default);
	RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
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
	m_Irradiance->Attach(4);

	m_PBRShader->Attach();
	m_PBRShader->SetFloat3("u_CameraPos", transform.Position);

	RenderCommand::DrawIndexed(m_Body.GetVertexArray());

	// Skybox
	{
		RenderCommand::SetCullFace(RenderCullFace::Front);
		RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

		glm::mat4 rotate = glm::toMat4(transform.Orientation);
		glm::mat4 view = glm::translate(rotate, (glm::vec3)-transform.Position);

		m_SkyboxShader->Attach();
		m_SkyboxShader->SetMat4("u_View", view);
		m_SkyboxShader->SetMat4("u_Projection", camera.GetProjection());
		m_HDRCubemap->Attach(0);

		RenderCommand::DrawIndexed(m_CubemapVAO);

		RenderCommand::SetCullFace(RenderCullFace::Default);
		RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
	}

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
