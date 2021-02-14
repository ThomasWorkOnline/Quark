///////////////////////////////////////////////////////////////////////////////////////////////
// 1. #include "Entropy.h"                                                                   //
// 2. Create a sub class that inherits from Entropy::Application                             //
// 3. Call Entropy::CreateApplication() returns your class instance                          //
// 4. Implement OnUpdate() and OnCreate() pure virtual methods                               //
// You're set!                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////

#include <Entropy.h>

struct Statistics
{
	size_t MemoryUsage = 0;
	uint32_t Allocations = 0;
	uint32_t Deallocations = 0;
};

static Statistics s_Stats;


void* operator new (size_t size) noexcept
{
	s_Stats.MemoryUsage += size;
	s_Stats.Allocations++;
	return malloc(size);
}

void* operator new[](size_t size) noexcept
{
	s_Stats.MemoryUsage += size;
	s_Stats.Allocations++;
	return malloc(size);
}

void operator delete (void* block, size_t size) noexcept
{
	s_Stats.MemoryUsage -= size;
	s_Stats.Deallocations++;
	free(block);
}

void operator delete[](void* block, size_t size) noexcept
{
	s_Stats.MemoryUsage -= size;
	s_Stats.Deallocations++;
	free(block);
}

class SandboxGame : public Entropy::Application
{
public:
	void OnCreate() override
	{
		auto& window = GetWindow();

		Entropy::BufferLayout layout = {
				{ Entropy::ShaderDataType::Float3, "a_Position" },
				{ Entropy::ShaderDataType::Float2, "a_TexCoord" },
				{ Entropy::ShaderDataType::Float3, "a_Normal" }
		};

		{
			m_CameraEntity.AddComponent<Entropy::TransformComponent>().Position = { 0.0f, 1.0f, -8.0f };
			m_CameraEntity.AddComponent<Entropy::PhysicsComponent>();
			m_CameraEntity.AddComponent<Entropy::CameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), 50.0f);
		}

		{
			m_PortalCamera.AddComponent<Entropy::TransformComponent>().Position = { 0.0f, 0.0f, -6.0f };
			m_PortalCamera.AddComponent<Entropy::PhysicsComponent>();
			m_PortalCamera.AddComponent<Entropy::CameraComponent>(1.0f, 45.0f);
		}

		{
			m_PlaneEntity.AddComponent<Entropy::TransformComponent>();
			m_PlaneEntity.AddComponent<Entropy::MeshComponent>().Mesh.GenerateTerrain(layout, 10, 0);
		}

		{
			auto& transform = m_ModelEntity.AddComponent<Entropy::TransformComponent>();
			//transform.Scale = { 0.01f, 0.01f, 0.01f };
			transform.Position.y += 1.3f;
			//m_ModelEntity.AddComponent<Entropy::MeshComponent>(layout, "./assets/models/ironman.obj");
			m_ModelEntity.AddComponent<Entropy::MeshComponent>().Mesh.GenerateUnitCube(layout);
		}

		{
			auto& transform = m_LightEntity.AddComponent<Entropy::TransformComponent>();
			transform.Scale = { 0.01f, 0.01f, 0.01f };
			transform.Position = { -0.7, 1.8f, -4.3f };
			m_LightEntity.AddComponent<Entropy::MeshComponent>(layout, "./assets/models/sphere.obj");
		}

		m_ShaderLibrary.Load("default", "./assets/shaders/default.glsl");
		m_ShaderLibrary.Load("defaultFlat", "./assets/shaders/defaultFlat.glsl");
		m_ShaderLibrary.Load("debugNormal", "./assets/shaders/debugNormal.glsl");
		m_ShaderLibrary.Load("debugPosition", "./assets/shaders/debugPosition.glsl");
		m_ShaderLibrary.Load("skybox", "./assets/shaders/skybox.glsl");
		m_SelectedShader = m_ShaderLibrary.Get("default");
	}

	void OnUpdate(float elapsedTime) override
	{
#if 1
		// Update scene before updating the camera controller
		// The camera controller will use the updated components
		// The other way around would result in 1 frame offset from the actual scene data
		m_Scene.OnUpdate(elapsedTime);
		m_CameraController.OnUpdate(elapsedTime);

		static float accumulatedTime = 0.0f;
		accumulatedTime += elapsedTime;

		if (accumulatedTime > 1.0f)
		{
			std::cout << std::fixed << 1.0f / elapsedTime << " fps | frametime: " << elapsedTime * 1000 << "ms\n";
			accumulatedTime = 0.0f;

			auto& window = GetWindow();
			std::cout << "Window resolution: " << window.GetWidth() << "x" << window.GetHeight() << std::endl << std::endl;

			std::cout << s_Stats.MemoryUsage << " bytes allocated" << std::endl;
			std::cout << "Total allocations: " << s_Stats.Allocations << std::endl;
			std::cout << "Total deallocations: " << s_Stats.Deallocations << std::endl;
			std::cout << "Allocations not freed: " << s_Stats.Allocations - s_Stats.Deallocations << std::endl << std::endl;

			//auto& transform = m_CameraEntity.GetComponent<Entropy::TransformComponent>();
			//std::cout << "Position: " << transform.Position << std::endl;
			//std::cout << "Orientation: " << transform.Orientation << std::endl;
			//std::cout << "Transform: " << transform.Scale << std::endl << std::endl;
		}

		// TODO: have this part of a script
		{
			auto& position = m_LightEntity.GetComponent<Entropy::TransformComponent>().Position;
			static const float speed = 10.0f;
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

		{
			// TODO: render pipeline and proper light object
			m_SelectedShader->Attach();
			m_SelectedShader->SetInt("u_Material.diffuse", 0);
			m_SelectedShader->SetInt("u_Material.specular", 1);
			m_SelectedShader->SetInt("u_Material.normalMap", 2);
			m_SelectedShader->SetInt("u_Cubemap", 3);
			m_SelectedShader->SetFloat("u_Material.shininess", 512.0f);
			m_SelectedShader->SetFloat("u_Material.metalness", 1.0f);

			constexpr float lightPower = 1.0f;
			constexpr glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f) * lightPower;
			m_SelectedShader->SetFloat3("u_Light.ambient", lightColor * 0.005f);
			m_SelectedShader->SetFloat3("u_Light.diffuse", lightColor * 0.2f);
			m_SelectedShader->SetFloat3("u_Light.specular", lightColor);
			// Light will cover a distance of 50 meters
			m_SelectedShader->SetFloat("u_Light.constant", 1.0f);
			m_SelectedShader->SetFloat("u_Light.linear", 0.09f);
			m_SelectedShader->SetFloat("u_Light.quadratic", 0.032f);

			m_SelectedShader->SetFloat("u_NormalLength", m_NormalLength);

			m_Skybox.GetCubeMap()->Attach(3);
		}

		{
			auto shader = m_ShaderLibrary.Get("skybox");
			shader->Attach();
			shader->SetInt("u_Cubemap", 0);
		}

		auto& transform = m_ModelEntity.GetComponent<Entropy::TransformComponent>();
		//transform.Rotate(elapsedTime * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
		static float accT = 0; accT += elapsedTime;
		transform.Position.x = sinf(accT * 4) * 3;

		/*
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
		*/

		{
			Entropy::Renderer::BeginScene(m_CameraEntity);

			white->Attach(0);
			white->Attach(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_ModelEntity);

			Entropy::Renderer::Submit(m_SelectedShader, m_LightEntity);

			m_SelectedShader->SetFloat("u_Material.metalness", 0.0f);
			//m_Framebuffer->AttachColorAttachment(0);
			//m_Framebuffer->AttachColorAttachment(1);
			diffuseMap->Attach(0);
			specularMap->Attach(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_PlaneEntity);

			Entropy::Renderer::EndScene();
		}

		{
			glm::mat4 view = (glm::mat3)m_CameraEntity.GetComponent<Entropy::TransformComponent>();
			Entropy::Renderer::BeginScene(m_CameraEntity.GetComponent<Entropy::CameraComponent>().Camera.GetProjectionMatrix(), view);

			Entropy::RenderCommand::SetCullFace(Entropy::RenderCullFace::Back);
			Entropy::RenderCommand::SetDepthFunction(Entropy::RenderDepthFunction::LessEqual);

			m_Skybox.GetCubeMap()->Attach(0);
			Entropy::Renderer::Submit(m_ShaderLibrary.Get("skybox"), m_Skybox.GetVertexArray());

			Entropy::RenderCommand::SetCullFace(Entropy::RenderCullFace::Front);
			Entropy::RenderCommand::SetDepthFunction(Entropy::RenderDepthFunction::Less);

			Entropy::Renderer::EndScene();
		}
#endif
	}

	void OnDestroy() override
	{

	}

	void OnEvent(Entropy::Event& e) override
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
		case Entropy::Key::F1:
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
		case Entropy::Key::F2:
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
		case Entropy::Key::F3:
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
		case Entropy::Key::F4:
		{
			static uint32_t environmentIndex = 0;
			environmentIndex++;

			if (environmentIndex >= 5)
				environmentIndex = 0;

			switch (environmentIndex)
			{
			case 0:
				m_Skybox = Entropy::Environment("./assets/environments/FishermansBastion");
				break;
			case 1:
				m_Skybox = Entropy::Environment("./assets/environments/Lycksele3");
				break;
			case 2:
				m_Skybox = Entropy::Environment("./assets/environments/GoldenGateBridge");
				break;
			case 3:
				m_Skybox = Entropy::Environment("./assets/environments/GoldenGateBridge2");
				break;
			case 4:
				m_Skybox = Entropy::Environment("./assets/environments/Storforsen4");
				break;
			}

			NT_TRACE("Switched environment");

			break;
		}
		case Entropy::Key::F11:
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
		case Entropy::Key::Escape:
		{
			// Focus lost
			GetWindow().Deselect();
			break;
		}
		}

		return false;
	}

private:
	Entropy::Scene m_Scene;

	Entropy::Entity m_PlaneEntity = m_Scene.CreateEntity();
	Entropy::Entity m_ModelEntity = m_Scene.CreateEntity();
	Entropy::Entity m_LightEntity = m_Scene.CreateEntity();
	Entropy::Entity m_CameraEntity = m_Scene.CreateEntity();
	Entropy::Entity m_PortalCamera = m_Scene.CreateEntity();

	Entropy::Ref<Entropy::Texture2D> white = Entropy::Texture2D::Create("./assets/textures/white.jpg");
	Entropy::Ref<Entropy::Texture2D> diffuseMap = Entropy::Texture2D::Create("./assets/textures/container.png");
	Entropy::Ref<Entropy::Texture2D> specularMap = Entropy::Texture2D::Create("./assets/textures/container_specular.png");
	Entropy::Ref<Entropy::Texture2D> normalMap = Entropy::Texture2D::Create("./assets/textures/normal_map.png");
	Entropy::Ref<Entropy::Texture2D> cobblestone = Entropy::Texture2D::Create("./assets/textures/cobblestone.png");

	Entropy::ShaderLibrary m_ShaderLibrary;
	Entropy::Ref<Entropy::Shader> m_SelectedShader;
	float m_NormalLength = 0.1f;

	Entropy::Environment m_Skybox = Entropy::Environment("./assets/environments/FishermansBastion");

	Entropy::CameraController m_CameraController = Entropy::CameraController(m_CameraEntity);

	Entropy::FramebufferSpecification m_FramebufferSpec = { 1024, 1024, 1, {
		{ Entropy::TextureDataFormat::RGB8, Entropy::TextureFilteringFormat::Linear, Entropy::TextureTilingFormat::Repeat },
		{ Entropy::TextureDataFormat::Depth24Stencil8 } } };
	Entropy::Ref<Entropy::Framebuffer> m_Framebuffer = Entropy::Framebuffer::Create(m_FramebufferSpec);
};

int main()
{
	auto app = new SandboxGame();
	app->Run();
	delete app;

	return 0;
}
