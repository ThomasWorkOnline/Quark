#include "PBRRendering.h"

static Ref<Texture2D> CreateTextureFromImage(const Ref<Image>& image, const Texture2DSpecification& spec)
{
	auto texture = Texture2D::Create(spec);
	texture->SetData(image->Data(), image->Size());
	texture->GenerateMipmaps();
	return texture;
}

PBRRendering::PBRRendering()
{
	m_Player = m_Scene.CreateEntity();
	m_Player.AddComponent<Transform3DComponent>().Position = { 0.0f, 0.0f, -2.0f };
	m_Player.AddComponent<PhysicsComponent>().Friction = 4.0f;
	m_Player.AddComponent<PerspectiveCameraComponent>((float)GetWindow().GetWidth() / GetWindow().GetHeight(), 70.0f);
	m_Controller = m_Player;

	{
		MeshFormatDescriptor md;
		m_MeshDataFuture = std::async(std::launch::async, Mesh::ReadOBJData, "assets/meshes/poly_sphere.obj", md);
	}

	{
		Mesh cube = Mesh::GenerateUnitCube();
		m_CubemapVAO = cube.GetVertexArray();
	}

#define MATERIAL 0

	{
		const char* albedoFilepath    = nullptr;
		const char* metallicFilepath  = nullptr;
		const char* normalFilepath    = nullptr;
		const char* roughnessFilepath = nullptr;
		const char* aoFilepath        = nullptr;

#if MATERIAL == 0
		albedoFilepath    = "assets/textures/pbr/greasy-pan/greasy-pan-2-albedo.png";
		metallicFilepath  = "assets/textures/pbr/greasy-pan/greasy-pan-2-metal.png";
		normalFilepath    = "assets/textures/pbr/greasy-pan/greasy-pan-2-normal.png";
		roughnessFilepath = "assets/textures/pbr/greasy-pan/greasy-pan-2-roughness.png";
#endif

#if MATERIAL == 1
		albedoFilepath    = "assets/textures/pbr/streaked-metal/albedo.png";
		metallicFilepath  = "assets/textures/pbr/streaked-metal/metalness.png";
		normalFilepath    = "assets/textures/pbr/streaked-metal/normal-dx.png";
		roughnessFilepath = "assets/textures/pbr/streaked-metal/rough.png";
#endif

#if MATERIAL == 2
		albedoFilepath    = "assets/textures/pbr/rustediron/rustediron2_basecolor.png";
		metallicFilepath  = "assets/textures/pbr/rustediron/rustediron2_metallic.png";
		normalFilepath    = "assets/textures/pbr/rustediron/rustediron2_normal.png";
		roughnessFilepath = "assets/textures/pbr/rustediron/rustediron2_roughness.png";
#endif

#if MATERIAL == 3
		albedoFilepath    = "assets/textures/pbr/gray-granite/gray-granite-flecks-albedo.png";
		metallicFilepath  = "assets/textures/pbr/gray-granite/gray-granite-flecks-Metallic.png";
		normalFilepath    = "assets/textures/pbr/gray-granite/gray-granite-flecks-Normal-dx.png";
		roughnessFilepath = "assets/textures/pbr/gray-granite/gray-granite-flecks-Roughness.png";
		aoFilepath        = "assets/textures/pbr/gray-granite/gray-granite-flecks-ao.png";
#endif

#if MATERIAL == 4
		albedoFilepath    = "assets/textures/pbr/marble-speckled/marble-speckled-albedo.png";
		metallicFilepath  = "assets/textures/pbr/marble-speckled/marble-speckled-metalness.png";
		normalFilepath    = "assets/textures/pbr/marble-speckled/marble-speckled-normal.png";
		roughnessFilepath = "assets/textures/pbr/marble-speckled/marble-speckled-roughness.png";
		aoFilepath        = "assets/textures/pbr/gray-granite/gray-granite-flecks-ao.png";
#endif

#if 1
		m_AlbedoFuture    = std::async(std::launch::async, Image::Create, albedoFilepath);
		m_MetallicFuture  = std::async(std::launch::async, Image::Create, metallicFilepath);
		m_NormalFuture    = std::async(std::launch::async, Image::Create, normalFilepath);
		m_RoughnessFuture = std::async(std::launch::async, Image::Create, roughnessFilepath);
#else
		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::RGBA;
			spec.InternalFormat = TextureInternalFormat::RGBA8;

			uint32_t data = 0xff0000ff;

			m_Albedo = Texture2D::Create(spec);
			m_Albedo->SetData(&data, sizeof(uint32_t));
		}

		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::Red;
			spec.InternalFormat = TextureInternalFormat::Red8;

			uint8_t data = 0xff;

			m_Metallic = Texture2D::Create(spec);
			m_Metallic->SetData(&data, sizeof(uint8_t));
		}

		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::Red;
			spec.InternalFormat = TextureInternalFormat::Red8;

			uint8_t data = 0x20;

			m_Roughness = Texture2D::Create(spec);
			m_Roughness->SetData(&data, sizeof(uint8_t));
		}

		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::RGB;
			spec.InternalFormat = TextureInternalFormat::RGB8;

			uint8_t data[3] = { 0x00, 0x00, 0xff };

			m_Normal = Texture2D::Create(spec);
			m_Normal->SetData(&data, 3 * sizeof(uint8_t));
		}
#endif

		if (aoFilepath)
		{
			m_AOFuture = std::async(std::launch::async, Image::Create, aoFilepath);
		}
		else
		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::Red;
			spec.InternalFormat = TextureInternalFormat::Red8;

			uint8_t data = 0xff;

			m_AO = Texture2D::Create(spec);
			m_AO->SetData(&data, sizeof(uint8_t));
		}
	}

	m_HDRTexture       = Texture2D::Create("assets/textures/hdr/MonValley_G_DirtRoad_3k.hdr");
	m_PBRShader        = Shader::Create("assets/shaders/PBR.glsl");
	m_IrradianceShader = Shader::Create("assets/shaders/irradiance.glsl");
	m_EquirectangleToCubemapShader = Shader::Create("assets/shaders/equirectangleToCubemap.glsl");
	m_SkyboxShader     = Shader::Create("assets/shaders/cubemap.glsl");

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
	m_PBRShader->SetInt("u_AmbiantOcclusionMap", 4);
	m_PBRShader->SetInt("u_IrradianceMap", 5);

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

	{
		CubemapSpecification spec;
		spec.Width          = 2048;
		spec.Height         = 2048;
		m_HDRCubemap = Cubemap::Create(spec);

		spec.Width          = 32;
		spec.Height         = 32;
		m_Irradiance = Cubemap::Create(spec);
	}

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

	// Due to the lack of a render pipeline, we reset the viewport after detaching the fbo
	auto& window = GetWindow();
	RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
	RenderCommand::SetCullFace(RenderCullFace::Default);
	RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
}

void PBRRendering::OnUpdate(Timestep elapsedTime)
{
	m_Controller.OnUpdate(elapsedTime);
	m_Scene.OnUpdate(elapsedTime);
}

void PBRRendering::OnRender()
{
	UploadAssets();

	const auto& camera = m_Player.GetComponent<PerspectiveCameraComponent>().Camera;
	const auto& transform = m_Player.GetComponent<Transform3DComponent>();

	Renderer::BeginScene(camera, transform);

	if (m_Albedo) m_Albedo->Attach(0);
	if (m_Normal) m_Normal->Attach(1);
	if (m_Metallic) m_Metallic->Attach(2);
	if (m_Roughness) m_Roughness->Attach(3);
	if (m_AO) m_AO->Attach(4);
	m_Irradiance->Attach(5);

	m_PBRShader->Attach();
	m_PBRShader->SetFloat3("u_CameraPos", transform.Position);

	if (m_Body)
	{
		RenderCommand::DrawIndexed(m_Body.GetVertexArray());
	}

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

void PBRRendering::UploadAssets()
{
	// NOTE: std::future::_Is_ready() is MSVC exclusive
	if (m_MeshDataFuture.valid() && m_MeshDataFuture._Is_ready())
	{
		OBJMeshData meshData = std::move(m_MeshDataFuture.get());
		m_Body = meshData;
	}

	if (m_AlbedoFuture.valid() && m_AlbedoFuture._Is_ready())
	{	
		Ref<Image> image = m_AlbedoFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? TextureDataFormat::RGBA : TextureDataFormat::RGB;
		spec.InternalFormat = image->Channels() == 4 ? TextureInternalFormat::SRGBA8 : TextureInternalFormat::SRGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Albedo = CreateTextureFromImage(image, spec);
	}

	if (m_MetallicFuture.valid() && m_MetallicFuture._Is_ready())
	{
		Ref<Image> image = m_MetallicFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? TextureDataFormat::RGBA : TextureDataFormat::RGB;
		spec.InternalFormat = image->Channels() == 4 ? TextureInternalFormat::RGBA8 : TextureInternalFormat::RGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Metallic = CreateTextureFromImage(image, spec);
	}

	if (m_NormalFuture.valid() && m_NormalFuture._Is_ready())
	{
		Ref<Image> image = m_NormalFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? TextureDataFormat::RGBA : TextureDataFormat::RGB;
		spec.InternalFormat = image->Channels() == 4 ? TextureInternalFormat::RGBA8 : TextureInternalFormat::RGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Normal = CreateTextureFromImage(image, spec);
	}

	if (m_RoughnessFuture.valid() && m_RoughnessFuture._Is_ready())
	{
		Ref<Image> image = m_RoughnessFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? TextureDataFormat::RGBA : TextureDataFormat::RGB;
		spec.InternalFormat = image->Channels() == 4 ? TextureInternalFormat::RGBA8 : TextureInternalFormat::RGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Roughness = CreateTextureFromImage(image, spec);
	}

	if (m_AOFuture.valid() && m_AOFuture._Is_ready())
	{
		Ref<Image> image = m_AOFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? TextureDataFormat::RGBA : TextureDataFormat::RGB;
		spec.InternalFormat = image->Channels() == 4 ? TextureInternalFormat::RGBA8 : TextureInternalFormat::RGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_AO = CreateTextureFromImage(image, spec);
	}
}
