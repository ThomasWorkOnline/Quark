#include "PBRRenderingDemo.h"

#include <Quark/Scripts/CameraController.h>

static Ref<Texture2D> CreateTextureFromImage(const Ref<Image>& image, const Texture2DSpecification& spec)
{
	auto texture = Texture2D::Create(spec);
	texture->SetData(image->Data(), image->Size());
	texture->GenerateMipmaps();
	return texture;
}

PBRRenderingDemo::PBRRenderingDemo()
{
	QK_PROFILE_FUNCTION();

	GetWindow()->SetVSync(true);

	m_Scene = Scene::Create();
	m_Player = m_Scene->CreateEntity();
	m_Player.AddComponent<Transform3DComponent>().Position = { 0.0f, 0.0f, -2.0f };
	m_Player.AddComponent<PhysicsComponent>();
	m_Player.AddComponent<CameraComponent>().Camera.SetPerspective(70.0f);
	m_Player.AddNativeScript<CameraController>();

	m_Scene->SetPrimaryCamera(m_Player);

	{
		MeshFormatDescriptor md;
		m_MeshDataFuture = std::async(std::launch::async, Mesh::ReadOBJData, "assets/meshes/poly_sphere.obj", md);
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
		m_AlbedoFuture    = std::async(std::launch::async, Image::Shared, albedoFilepath);
		m_MetallicFuture  = std::async(std::launch::async, Image::Shared, metallicFilepath);
		m_NormalFuture    = std::async(std::launch::async, Image::Shared, normalFilepath);
		m_RoughnessFuture = std::async(std::launch::async, Image::Shared, roughnessFilepath);
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
			m_AOFuture = std::async(std::launch::async, Image::Shared, aoFilepath);
		}
		else
		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = ColorDataFormat::Red8;

			uint8_t data = 0xff;

			m_AO = Texture2D::Create(spec);
			m_AO->SetData(&data, sizeof(uint8_t));
		}
	}

	m_PBRShader = Shader::Create("assets/shaders/PBR.glsl");

	static constexpr float lightPower = 10.0f;
	static constexpr Vec3f lightColor = Vec3f(1.0f, 1.0f, 1.0f) * lightPower;
	static constexpr Vec3f lightPositions[4] = {
		{  0.0f,  0.0f, -3.0f },
		{  0.0f,  2.0f,  3.0f },
		{ -1.7f,  0.3f, -0.5f },
		{ -2.0f,  2.0f,  2.0f }
	};

	m_PBRShader->Attach();
	m_PBRShader->SetMat4f("u_Model", Mat4f(1.0f));
	m_PBRShader->SetInt("u_AlbedoMap", 0);
	m_PBRShader->SetInt("u_NormalMap", 1);
	m_PBRShader->SetInt("u_MetallicMap", 2);
	m_PBRShader->SetInt("u_RoughnessMap", 3);
	m_PBRShader->SetInt("u_AmbiantOcclusionMap", 4);
	m_PBRShader->SetInt("u_IrradianceMap", 5);

	m_PBRShader->SetVec3f("u_LightColors[0]", lightColor);
	m_PBRShader->SetVec3f("u_LightColors[1]", lightColor);
	m_PBRShader->SetVec3f("u_LightColors[2]", lightColor);
	m_PBRShader->SetVec3f("u_LightColors[3]", lightColor);

	m_PBRShader->SetVec3f("u_LightPositions[0]", lightPositions[0]);
	m_PBRShader->SetVec3f("u_LightPositions[1]", lightPositions[1]);
	m_PBRShader->SetVec3f("u_LightPositions[2]", lightPositions[2]);
	m_PBRShader->SetVec3f("u_LightPositions[3]", lightPositions[3]);
}

void PBRRenderingDemo::OnUpdate(Timestep elapsedTime)
{
	m_Scene->OnUpdate(elapsedTime);
}

void PBRRenderingDemo::OnRender()
{
	Timer t;
	t.Start();

	UploadAssets();

	if (m_Albedo) m_Albedo->Attach(0);
	if (m_Normal) m_Normal->Attach(1);
	if (m_Metallic) m_Metallic->Attach(2);
	if (m_Roughness) m_Roughness->Attach(3);
	if (m_AO) m_AO->Attach(4);

	const auto& camera = m_Player.GetComponent<CameraComponent>().Camera;
	const auto& transform = m_Player.GetComponent<Transform3DComponent>();

	m_PBRShader->Attach();
	m_PBRShader->SetVec3f("u_CameraPos", transform.Position);

	//Renderer::BeginScene(camera, transform);

	if (m_Body)
	{
		//RenderCommand::DrawIndexed(m_Body.GetVertexArray());
	}

	//Renderer::EndScene();

	t.Stop();
	auto ms = t.Microseconds().count() / 1000.0f;
	QK_CORE_INFO("Frametime: {0}ms", ms);
}

void PBRRenderingDemo::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(PBRRenderingDemo::OnKeyPressed));
	dispatcher.Dispatch<MouseMovedEvent>(ATTACH_EVENT_FN(PBRRenderingDemo::OnMouseMoved));
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(PBRRenderingDemo::OnMouseButtonPressed));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(ATTACH_EVENT_FN(PBRRenderingDemo::OnMouseButtonReleased));

	e.Handled = e.IsInCategory(EventCategory::Input) && GetWindow()->IsCursorEnabled();

	if (!e.Handled)
		m_Scene->OnEvent(e);
}

bool PBRRenderingDemo::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::Escape:
			auto window = GetWindow();
			window->EnableCursor();
			break;
	}
	return false;
}

bool PBRRenderingDemo::OnMouseMoved(MouseMovedEvent& e)
{
	return !GetWindow()->IsCursorEnabled();
}

bool PBRRenderingDemo::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	GetWindow()->DisableCursor();
	return false;
}

bool PBRRenderingDemo::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	return false;
}

void PBRRenderingDemo::UploadAssets()
{
	if (m_MeshDataFuture.valid() && m_MeshDataFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		OBJMeshData meshData = std::move(m_MeshDataFuture.get());
		m_Body = meshData;
	}

	if (m_AlbedoFuture.valid() && m_AlbedoFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{	
		Ref<Image> image = m_AlbedoFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? ColorDataFormat::RGBA8_SRGB : ColorDataFormat::RGB8_SRGB;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Albedo = CreateTextureFromImage(image, spec);
	}

	if (m_MetallicFuture.valid() && m_MetallicFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		Ref<Image> image = m_MetallicFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? ColorDataFormat::RGBA8 : ColorDataFormat::RGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Metallic = CreateTextureFromImage(image, spec);
	}

	if (m_NormalFuture.valid() && m_NormalFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		Ref<Image> image = m_NormalFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? ColorDataFormat::RGBA8 : ColorDataFormat::RGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Normal = CreateTextureFromImage(image, spec);
	}

	if (m_RoughnessFuture.valid() && m_RoughnessFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		Ref<Image> image = m_RoughnessFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? ColorDataFormat::RGBA8 : ColorDataFormat::RGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_Roughness = CreateTextureFromImage(image, spec);
	}

	if (m_AOFuture.valid() && m_AOFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		Ref<Image> image = m_AOFuture.get();

		Texture2DSpecification spec;
		spec.Width = image->Width();
		spec.Height = image->Height();
		spec.DataFormat = image->Channels() == 4 ? ColorDataFormat::RGBA8 : ColorDataFormat::RGB8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Linear;
		spec.RenderModes.MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;

		m_AO = CreateTextureFromImage(image, spec);
	}
}
