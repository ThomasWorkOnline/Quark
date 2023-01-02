#include "PBRRenderingDemo.h"

#include <Quark/Scripts/CameraController.h>

#define MATERIAL 0

PBRRenderingDemo::PBRRenderingDemo(const ApplicationSpecification& spec)
	: Application(spec)
{
	QK_PROFILE_FUNCTION();

	GetWindow()->SetVSync(true);

	m_Scene.GetSettings().AirDensity = 4.0f;
	m_Scene.OnPlay();

	m_CameraEntity = m_Scene.CreatePrimaryCamera();
	m_CameraEntity.GetComponent<Transform3DComponent>().Position = { 0.0f, 0.0f, -2.0f };
	m_CameraEntity.AddNativeScript<CameraController>();

	m_MeshDataFuture = std::async(std::launch::async, StaticMesh::ReadOBJData, "assets/Models/poly_sphere.obj");
	LoadMaterialsAsync();

	m_SceneRenderer.SetEnvironment("assets/Environments/MonValley_G_DirtRoad_3k.hdr");
	m_SceneRenderer.SetPrimaryCamera(m_CameraEntity);

	{
		CubemapSpecification spec;
		spec.Width = 1;
		spec.Height = 1;
		spec.DataFormat = ColorFormat::RGB32f;

		m_Irradiance = Cubemap::Create(spec);

		float data[3] = { 1.0f, 1.0f, 1.0f };
		for (auto i = 0; i < 6; i++)
		{
			m_Irradiance->SetData(data, sizeof(data), i);
		}
	}

	//m_PBRShader = Shader::CreateLegacy("assets/Shaders/PBR.glsl");
	m_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraUniformBufferData));

	for (auto i = 0; i < sizeof_array(m_Samplers); i++)
	{
		SamplerSpecification spec;
		spec.RenderModes.MinFilteringMode = SamplerFilterMode::Linear;
		spec.RenderModes.MagFilteringMode = SamplerFilterMode::Linear;
		spec.RenderModes.MipmapMode = SamplerMipmapMode::Linear;
		m_Samplers[i] = Sampler::Create(spec);
	}

	{
		PipelineSpecification spec;
		spec.Layout             = StaticMesh::GetBufferLayout();
		spec.Topology           = PrimitiveTopology::TriangleList;
		spec.DescriptorSetCount = Renderer::GetCurrentFrameIndex();
		spec.Shader             = m_PBRShader.get();
		spec.RenderPass         = Renderer::GetRenderPass();

		m_Pipeline = Pipeline::Create(spec);
	}

	static constexpr float lightPower = 10.0f;
	static constexpr Vec3f lightColor = Vec3f(1.0f, 1.0f, 1.0f) * lightPower;
	static constexpr Vec3f lightPositions[4] = {
		{  0.0f,  0.0f, -3.0f },
		{  0.0f,  2.0f,  3.0f },
		{ -1.7f,  0.3f, -0.5f },
		{ -2.0f,  2.0f,  2.0f }
	};

	m_PBRShader->SetInt("u_Material.AlbedoMap", 0);
	m_PBRShader->SetInt("u_Material.NormalMap", 1);
	m_PBRShader->SetInt("u_Material.MetallicMap", 2);
	m_PBRShader->SetInt("u_Material.RoughnessMap", 3);
	m_PBRShader->SetInt("u_Material.AmbiantOcclusionMap", 4);
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

PBRRenderingDemo::~PBRRenderingDemo()
{
	m_Scene.OnStop();
}

void PBRRenderingDemo::OnUpdate(Timestep elapsedTime)
{
	m_Scene.OnUpdate(elapsedTime);
}

void PBRRenderingDemo::OnRender()
{
	UploadAssets();

	const auto& camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;
	const auto& transform = m_CameraEntity.GetComponent<Transform3DComponent>();

	Mat4f cameraRotate = glm::toMat4(transform.Orientation);
	Mat4f cameraView = glm::translate(cameraRotate, (Vec3f)-transform.Position);

	m_CameraBufferData.ViewProjection = camera.GetProjection() * cameraView;
	m_CameraBufferData.CameraPosition = Vec4f(transform.Position, 1.0f);
	m_CameraUniformBuffer->SetData(&m_CameraBufferData, sizeof(m_CameraBufferData));

	if (m_Body)
	{
		Renderer::BindPipeline(m_Pipeline.get());

		Renderer::BindUniformBuffer(m_CameraUniformBuffer.get(), 0);
		Renderer::BindTexture(m_Irradiance.get(), m_Samplers[5].get(), 0, 5);

		if (m_Material.Albedo)    Renderer::BindTexture(m_Material.Albedo.get(),    m_Samplers[0].get(), 0, 0);
		if (m_Material.Normal)    Renderer::BindTexture(m_Material.Normal.get(),    m_Samplers[1].get(), 0, 1);
		if (m_Material.Metallic)  Renderer::BindTexture(m_Material.Metallic.get(),  m_Samplers[2].get(), 0, 2);
		if (m_Material.Roughness) Renderer::BindTexture(m_Material.Roughness.get(), m_Samplers[3].get(), 0, 3);
		if (m_Material.AO)        Renderer::BindTexture(m_Material.AO.get(),        m_Samplers[4].get(), 0, 4);

		Renderer::BindDescriptorSets();
		Renderer::DrawIndexed(m_Body.VertexBuffer.get(), m_Body.IndexBuffer.get());
	}

	m_SceneRenderer.OnRender();
}

void PBRRenderingDemo::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<MouseMovedEvent>(ATTACH_EVENT_FN(OnMouseMoved));
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(OnMouseButtonPressed));

	e.Handled = e.IsInCategory(EventCategory::Input) && GetWindow()->IsCursorEnabled();

	if (!e.Handled)
		m_Scene.OnEvent(e);

	DefaultEventHandler(e);
}

bool PBRRenderingDemo::OnMouseMoved(MouseMovedEvent& e)
{
	return GetWindow()->IsCursorEnabled();
}

void PBRRenderingDemo::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	GetWindow()->DisableCursor();
}

void PBRRenderingDemo::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::Escape:
		{
			auto window = GetWindow();
			window->EnableCursor();
			break;
		}
		default:
			break;
	}
}

static Scope<Texture2D> CreateTextureFromImage(const Image& image, const Texture2DSpecification& spec)
{
	auto texture = Texture2D::Create(spec);
	texture->SetData(image.GetData(), image.GetSize());
	texture->GenerateMipmaps();
	return texture;
}

void PBRRenderingDemo::LoadMaterialsAsync()
{
	{
		const char* albedoFilepath = nullptr;
		const char* metallicFilepath = nullptr;
		const char* normalFilepath = nullptr;
		const char* roughnessFilepath = nullptr;
		const char* aoFilepath = nullptr;

#if MATERIAL == 0
		albedoFilepath = "assets/Textures/pbr/greasy-pan/greasy-pan-2-albedo.png";
		metallicFilepath = "assets/Textures/pbr/greasy-pan/greasy-pan-2-metal.png";
		normalFilepath = "assets/Textures/pbr/greasy-pan/greasy-pan-2-normal.png";
		roughnessFilepath = "assets/Textures/pbr/greasy-pan/greasy-pan-2-roughness.png";
#endif

#if MATERIAL == 1
		albedoFilepath = "assets/Textures/pbr/streaked-metal/albedo.png";
		metallicFilepath = "assets/Textures/pbr/streaked-metal/metalness.png";
		normalFilepath = "assets/Textures/pbr/streaked-metal/normal-dx.png";
		roughnessFilepath = "assets/Textures/pbr/streaked-metal/rough.png";
#endif

#if MATERIAL == 2
		albedoFilepath = "assets/Textures/pbr/rustediron/rustediron2_basecolor.png";
		metallicFilepath = "assets/Textures/pbr/rustediron/rustediron2_metallic.png";
		normalFilepath = "assets/Textures/pbr/rustediron/rustediron2_normal.png";
		roughnessFilepath = "assets/Textures/pbr/rustediron/rustediron2_roughness.png";
#endif

#if MATERIAL == 3
		albedoFilepath = "assets/Textures/pbr/gray-granite/gray-granite-flecks-albedo.png";
		metallicFilepath = "assets/Textures/pbr/gray-granite/gray-granite-flecks-Metallic.png";
		normalFilepath = "assets/Textures/pbr/gray-granite/gray-granite-flecks-Normal-dx.png";
		roughnessFilepath = "assets/Textures/pbr/gray-granite/gray-granite-flecks-Roughness.png";
		aoFilepath = "assets/Textures/pbr/gray-granite/gray-granite-flecks-ao.png";
#endif

#if MATERIAL == 4
		albedoFilepath = "assets/Textures/pbr/marble-speckled/marble-speckled-albedo.png";
		metallicFilepath = "assets/Textures/pbr/marble-speckled/marble-speckled-metalness.png";
		normalFilepath = "assets/Textures/pbr/marble-speckled/marble-speckled-normal.png";
		roughnessFilepath = "assets/Textures/pbr/marble-speckled/marble-speckled-roughness.png";
		aoFilepath = "assets/Textures/pbr/gray-granite/gray-granite-flecks-ao.png";
#endif

#if MATERIAL >= 5
#	error "Invalid material ID"
#endif

#if 1
		auto newImageLambda = [](const char* filepath)
		{
			return Image(filepath);
		};

		m_AlbedoFuture    = std::async(std::launch::async, newImageLambda, albedoFilepath);
		m_MetallicFuture  = std::async(std::launch::async, newImageLambda, metallicFilepath);
		m_NormalFuture    = std::async(std::launch::async, newImageLambda, normalFilepath);
		m_RoughnessFuture = std::async(std::launch::async, newImageLambda, roughnessFilepath);
#else

		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::RGBA;
			spec.InternalFormat = TextureInternalFormat::RGBA8;

			m_Albedo = Texture2D::Create(spec);

			uint32_t data = 0xff0000ff;
			m_Albedo->SetData(&data, sizeof(data));
		}

		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::Red;
			spec.InternalFormat = TextureInternalFormat::Red8;

			m_Metallic = Texture2D::Create(spec);

			uint8_t data = 0xff;
			m_Metallic->SetData(&data, sizeof(data));
		}

		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::Red;
			spec.InternalFormat = TextureInternalFormat::Red8;

			m_Roughness = Texture2D::Create(spec);

			uint8_t data = 0x20;
			m_Roughness->SetData(&data, sizeof(data));
		}

		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = TextureDataFormat::RGB;
			spec.InternalFormat = TextureInternalFormat::RGB8;

			m_Normal = Texture2D::Create(spec);

			uint8_t data[3] = { 0x00, 0x00, 0xff };
			m_Normal->SetData(data, sizeof(data));
		}
#endif

		if (aoFilepath)
		{
			m_AOFuture = std::async(std::launch::async, newImageLambda, aoFilepath);
		}
		else
		{
			Texture2DSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			spec.DataFormat = ColorFormat::Red8;
			m_Material.AO = Texture2D::Create(spec);

			uint8_t data = 0xff;
			m_Material.AO->SetData(&data, sizeof(data));
		}
	}
}

void PBRRenderingDemo::UploadAssets()
{
	if (m_MeshDataFuture.valid() && m_MeshDataFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		OBJMeshData meshData = m_MeshDataFuture.get();
		m_Body = meshData;
	}

	if (m_AlbedoFuture.valid() && m_AlbedoFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{	
		auto image = m_AlbedoFuture.get();

		Texture2DSpecification spec;
		spec.Width = image.Width();
		spec.Height = image.Height();
		spec.DataFormat = image.Channels() == 4 ? ColorFormat::RGBA8SRGB : ColorFormat::RGB8SRGB;
		m_Material.Albedo = CreateTextureFromImage(image, spec);
	}

	if (m_MetallicFuture.valid() && m_MetallicFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		auto image = m_MetallicFuture.get();

		Texture2DSpecification spec;
		spec.Width = image.Width();
		spec.Height = image.Height();
		spec.DataFormat = image.GetSpecification().DataFormat;
		m_Material.Metallic = CreateTextureFromImage(image, spec);
	}

	if (m_NormalFuture.valid() && m_NormalFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		auto image = m_NormalFuture.get();

		Texture2DSpecification spec;
		spec.Width = image.Width();
		spec.Height = image.Height();
		spec.DataFormat = image.GetSpecification().DataFormat;
		m_Material.Normal = CreateTextureFromImage(image, spec);
	}

	if (m_RoughnessFuture.valid() && m_RoughnessFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		auto image = m_RoughnessFuture.get();

		Texture2DSpecification spec;
		spec.Width = image.Width();
		spec.Height = image.Height();
		spec.DataFormat = image.GetSpecification().DataFormat;
		m_Material.Roughness = CreateTextureFromImage(image, spec);
	}

	if (m_AOFuture.valid() && m_AOFuture.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
	{
		auto image = m_AOFuture.get();

		Texture2DSpecification spec;
		spec.Width = image.Width();
		spec.Height = image.Height();
		spec.DataFormat = image.GetSpecification().DataFormat;
		m_Material.AO = CreateTextureFromImage(image, spec);
	}
}
