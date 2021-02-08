///////////////////////////////////////////////////////////////////////////////////////////////
// 1. #include "Entropy.h"                                                                   //
// 2. Create a sub class that inherits from Entropy::Application                             //
// 3. Call Entropy::CreateApplication() returns your class instance                          //
// 4. Implement OnUpdate() and OnCreate() pure virtual methods                               //
// You're set!                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Entropy.h"

static uint32_t LoadCubemap(std::string* faces)
{
	uint32_t renderedID;
	glGenTextures(1, &renderedID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderedID);
	stbi_set_flip_vertically_on_load(false);

	int width, height, channels;
	for (size_t i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			NT_ERROR("Cubemap tex failed to load at path: " << faces[i]);

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return renderedID;
}

static std::string files[6] = {
	"./assets/environments/Storforsen4/posx.jpg",
	"./assets/environments/Storforsen4/negx.jpg",
	"./assets/environments/Storforsen4/posy.jpg",
	"./assets/environments/Storforsen4/negy.jpg",
	"./assets/environments/Storforsen4/posz.jpg",
	"./assets/environments/Storforsen4/negz.jpg"
};

float cubemapVertices[] = {
   -0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
   -0.5f, -0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f, -0.5f,
   -0.5f,  0.5f, -0.5f
};

static uint32_t cubemapIndices[] = {
	// front
	0, 2, 3,
	1, 2, 0,

	// right
	4, 6, 7,
	5, 6, 4,

	// back
	8, 10, 11,
	9, 10, 8,

	// left
	12, 14, 15,
	13, 14, 12,

	// bottom
	16, 18, 19,
	17, 18, 16,

	// top
	20, 22, 23,
	21, 22, 20
};

class SandboxGame : public Entropy::Application
{
public:
	void OnCreate() override
	{
		auto& window = GetWindow();

		{
			m_CameraEntity.AddComponent<Entropy::TransformComponent>().Position = { 0.0f, 1.0f, -8.0f };
			m_CameraEntity.AddComponent<Entropy::PhysicsComponent>();
			m_CameraEntity.AddComponent<Entropy::CameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), 45.0f);
		}

		{
			m_PortalCamera.AddComponent<Entropy::TransformComponent>().Position = { 0.0f, 0.0f, -6.0f };
			m_PortalCamera.AddComponent<Entropy::PhysicsComponent>();
			m_PortalCamera.AddComponent<Entropy::CameraComponent>(1.0f, 45.0f);
		}

		{
			m_PlaneEntity.AddComponent<Entropy::TransformComponent>();
			m_PlaneEntity.AddComponent<Entropy::MeshComponent>().Mesh.GenerateTerrain(10, 0);
		}

		{
			m_ModelEntity.AddComponent<Entropy::TransformComponent>().Position.y = 1.5f;
			m_ModelEntity.AddComponent<Entropy::MeshComponent>("./assets/models/monkey.obj");
			//m_ModelEntity.AddComponent<Entropy::MeshComponent>().Mesh.GenerateUnitCube();
		}

		{
			auto& transform = m_LightEntity.AddComponent<Entropy::TransformComponent>();
			transform.Scale = { 0.01f, 0.01f, 0.01f };
			transform.Position = { -0.7, 1.8f, -4.3f };
			m_LightEntity.AddComponent<Entropy::MeshComponent>("./assets/models/sphere.obj");
		}

		m_ShaderLibrary.Load("default", "./assets/shaders/default.glsl");
		m_ShaderLibrary.Load("defaultFlat", "./assets/shaders/defaultFlat.glsl");
		m_ShaderLibrary.Load("debugNormal", "./assets/shaders/debugNormal.glsl");
		m_ShaderLibrary.Load("debugPosition", "./assets/shaders/debugPosition.glsl");
		m_ShaderLibrary.Load("skybox", "./assets/shaders/skybox.glsl");
		m_SelectedShader = m_ShaderLibrary.Get("default");

		m_CubeMapRendererID = LoadCubemap(files);

		Entropy::BufferLayout layout = {
			{ Entropy::ShaderDataType::Float3, "a_Position" },
		};
		vb->SetLayout(layout);
		va->AddVertexBuffer(vb);
		va->SetIndexBuffer(ib);
	}

	void OnUpdate(float elapsedTime) override
	{
		// Update scene before updating the camera controller
		// The camera controller will use the updated components
		// The other way around would result in 1 frame offset from the actual scene data
		m_Scene.OnUpdate(elapsedTime);
		m_CameraController.OnUpdate(elapsedTime);

#if 0
		static float accumulatedTime = 0.0f;
		accumulatedTime += elapsedTime;

		if (accumulatedTime > 1.0f)
		{
			std::cout << std::fixed << 1.0f / elapsedTime << " fps | frametime: " << elapsedTime * 1000 << "ms\n";
			accumulatedTime = 0.0f;

			auto& window = GetWindow();
			std::cout << "Window resolution: " << window.GetWidth() << "x" << window.GetHeight() << std::endl << std::endl;

			auto& transform = m_CameraEntity.GetComponent<Entropy::TransformComponent>();
			std::cout << "Position: " << transform.Position << std::endl;
			std::cout << "Orientation: " << transform.Orientation << std::endl;
			std::cout << "Transform: " << transform.Scale << std::endl << std::endl;

			std::cout << "Framebuffer size: " << m_Spec.Width << "x" << m_Spec.Height << std::endl << std::endl;
		}
#endif

		// TODO: have this part of a script
		{
			auto& position = m_LightEntity.GetComponent<Entropy::TransformComponent>().Position;
			const float speed = 10.0f;
			if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::RightShift))
			{
				if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Up))
				{
					position.y += elapsedTime * speed;
				}

				if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Down))
				{
					position.y -= elapsedTime * speed;
				}
			}
			else
			{
				if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Up))
				{
					position.z += elapsedTime * speed;
				}

				if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Down))
				{
					position.z -= elapsedTime * speed;
				}
			}

			if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Right))
			{
				position.x += elapsedTime * speed;
			}

			if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Left))
			{
				position.x -= elapsedTime * speed;
			}

			m_SelectedShader->Attach();
			m_SelectedShader->SetFloat3("u_Light.position", position);
		}

		if (Entropy::Input::IsMouseButtonPressed(Entropy::MouseCode::Button3))
		{
			m_NormalLength -= elapsedTime;

			if (m_NormalLength < 0.001f)
				m_NormalLength = 0.001f;
		}

		if (Entropy::Input::IsMouseButtonPressed(Entropy::MouseCode::Button4))
		{
			m_NormalLength += elapsedTime;

			if (m_NormalLength > 1.0f)
				m_NormalLength = 1.0f;
		}

		// TODO: render pipeline and proper light object
		m_SelectedShader->Attach();
		m_SelectedShader->SetInt("u_Material.diffuse", 0);
		m_SelectedShader->SetInt("u_Material.specular", 1);
		m_SelectedShader->SetInt("u_Material.normalMap", 2);
		m_SelectedShader->SetFloat("u_Material.shininess", 512.0f);

		constexpr float lightPower = 5.0f;
		constexpr glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f) * lightPower;
		m_SelectedShader->SetFloat3("u_Light.ambient", lightColor * 0.005f);
		m_SelectedShader->SetFloat3("u_Light.diffuse", lightColor * 0.2f);
		m_SelectedShader->SetFloat3("u_Light.specular", lightColor);
		// Light will cover a distance of 50 meters
		m_SelectedShader->SetFloat("u_Light.constant", 1.0f);
		m_SelectedShader->SetFloat("u_Light.linear", 0.09f);
		m_SelectedShader->SetFloat("u_Light.quadratic", 0.032f);

		m_SelectedShader->SetFloat("u_NormalLength", m_NormalLength);

		m_ModelEntity.GetComponent<Entropy::TransformComponent>().Rotate(elapsedTime, glm::vec3(0.0f, 1.0f, 0.0f));

		{
			Entropy::Renderer::BeginScene(m_PortalCamera);

			m_Framebuffer->Attach();

			// Drawing Suzanne to the framebuffer
			Entropy::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			Entropy::RenderCommand::Clear();
			white->Attach(0);
			white->Attach(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_ModelEntity);

			m_Framebuffer->Detach();
			Entropy::RenderCommand::SetViewport(0, 0, GetWindow().GetWidth(), GetWindow().GetHeight());

			Entropy::Renderer::EndScene();
		}

		{
			Entropy::Renderer::BeginScene(m_CameraEntity);

			white->Attach(0);
			white->Attach(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_ModelEntity);

			m_Framebuffer->AttachColorAttachment(0);
			m_Framebuffer->AttachColorAttachment(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_PlaneEntity);

			white->Attach(0);
			white->Attach(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_LightEntity);

			Entropy::Renderer::EndScene();
		}
		
		{
			glm::mat4 view = (glm::mat3)m_CameraEntity.GetComponent<Entropy::TransformComponent>();
			Entropy::Renderer::BeginScene(m_CameraEntity.GetComponent<Entropy::CameraComponent>().Camera.GetProjectionMatrix(), view);

			glCullFace(GL_BACK);
			glDepthFunc(GL_LEQUAL);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapRendererID);
			m_SelectedShader->SetInt("u_Cubemap", 3);
			Entropy::Renderer::Submit(m_ShaderLibrary.Get("skybox"), va);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			glCullFace(GL_FRONT);
			glDepthFunc(GL_LESS);

			Entropy::Renderer::EndScene();
		}
	}

	void OnApplicationEvent(Entropy::Event& e) override
	{
		Entropy::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Entropy::MouseButtonPressedEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnMouseButtonPressed));
		dispatcher.Dispatch<Entropy::KeyPressedEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnKeyPressed));
		dispatcher.Dispatch<Entropy::WindowResizeEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnWindowResized));

		m_CameraController.OnEvent(e);
	}

	bool OnWindowResized(Entropy::WindowResizeEvent& e)
	{
		return false;
	}

	bool OnMouseButtonPressed(Entropy::MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
		case Entropy::MouseCode::Button0:
		{
			// Focus enabled
			GetWindow().Select();
			break;
		}
		}
		return false;
	}

	bool OnKeyPressed(Entropy::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Entropy::KeyCode::F1:
		{
			// Toggle debug shader
			static uint32_t shaderIndex = 0;
			shaderIndex++;

			if (shaderIndex >= 4)
				shaderIndex = 0;

			switch (shaderIndex)
			{
			case 0:
				m_SelectedShader = m_ShaderLibrary.Get("default");
				break;
			case 1:
				m_SelectedShader = m_ShaderLibrary.Get("defaultFlat");
				break;
			case 2:
				m_SelectedShader = m_ShaderLibrary.Get("debugNormal");
				break;
			case 3:
				m_SelectedShader = m_ShaderLibrary.Get("debugPosition");
			}

			std::stringstream ss;
			ss << "Shader used: ";
			ss << m_SelectedShader->GetName();
			NT_TRACE(ss.str());
			break;
		}
		case Entropy::KeyCode::F2:
		{
			if (m_CameraController.HasCamera())
			{
				m_CameraController.DetachCamera();
				NT_TRACE("Detached camera from controller");
			}
			else
			{
				m_CameraController.AttachCamera(m_CameraEntity);
				NT_TRACE("Attached camera to controller");
			}
			break;
		}
		case Entropy::KeyCode::F3:
		{
			// Toggle VSync
			auto& window = GetWindow();
			window.SetVSync(!window.IsVSync());

			std::stringstream ss;
			ss << "VSync is now ";
			ss << (window.IsVSync() ? "enabled" : "disabled");
			NT_TRACE(ss.str());
			break;
		}
		case Entropy::KeyCode::F11:
		{
			auto& window = GetWindow();
			if (window.IsFullscreen())
			{
				window.DisableFullScreen();
				NT_TRACE("Disabled fullscreen");
			}
			else
			{
				window.EnableFullScreen();
				NT_TRACE("Enabled fullscreen");
			}
			break;
		}
		case Entropy::KeyCode::Escape:
		{
			// Focus lost
			GetWindow().Deselect();
			break;
		}
		}

		return false;
	}

private:
	Entropy::Scene m_Scene = Entropy::Scene();

	Entropy::Ref<Entropy::Texture2D> white = Entropy::Texture2D::Create("./assets/textures/white.jpg");
	Entropy::Ref<Entropy::Texture2D> diffuseMap = Entropy::Texture2D::Create("./assets/textures/container.png");
	Entropy::Ref<Entropy::Texture2D> specularMap = Entropy::Texture2D::Create("./assets/textures/container_specular.png");
	Entropy::Ref<Entropy::Texture2D> normalMap = Entropy::Texture2D::Create("./assets/textures/normal_map.png");
	Entropy::Ref<Entropy::Texture2D> cobblestone = Entropy::Texture2D::Create("./assets/textures/cobblestone.png");

	Entropy::ShaderLibrary m_ShaderLibrary;
	Entropy::Ref<Entropy::Shader> m_SelectedShader;
	float m_NormalLength = 0.1f;

	Entropy::Entity m_PlaneEntity = m_Scene.CreateEntity();
	Entropy::Entity m_ModelEntity = m_Scene.CreateEntity();
	Entropy::Entity m_LightEntity = m_Scene.CreateEntity();
	Entropy::Entity m_CameraEntity = m_Scene.CreateEntity();
	Entropy::Entity m_PortalCamera = m_Scene.CreateEntity();

	Entropy::CameraController m_CameraController = Entropy::CameraController(m_CameraEntity);

	Entropy::FramebufferSpecification m_Spec = { 1024, 1024, {
		{ Entropy::FramebufferTextureFormat::RGBA8, Entropy::FramebufferFilteringFormat::Nearest, Entropy::FramebufferTilingFormat::Repeat },
		{ Entropy::FramebufferTextureFormat::Depth24Stencil8 } } };
	Entropy::Ref<Entropy::Framebuffer> m_Framebuffer = Entropy::Framebuffer::Create(m_Spec);

	uint32_t m_CubeMapRendererID = 0;

	Entropy::Ref<Entropy::VertexArray> va = Entropy::VertexArray::Create();
	Entropy::Ref<Entropy::IndexBuffer> ib = Entropy::IndexBuffer::Create(cubemapIndices, sizeof(cubemapIndices) / sizeof(uint32_t));
	Entropy::Ref<Entropy::VertexBuffer> vb = Entropy::VertexBuffer::Create(cubemapVertices, 108 * sizeof(float));
	Entropy::BufferLayout layout;
};

Entropy::Application* Entropy::CreateApplication()
{
	return new SandboxGame();
}
