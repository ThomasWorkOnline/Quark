#include "VulkanApp.h"
#include "Quark/Scripts/CameraController.h"

VulkanApp::VulkanApp(const ApplicationOptions& options)
	: Application(options)
{
	m_Scene = CreateRef<PresentableScene>();
	m_CameraEntity = m_Scene->CreatePrimaryCamera();
	m_CameraEntity.AddNativeScript<CameraController>();

	Ref<Texture2D> texture1 = Texture2D::Create("assets/Textures/pbr/streaked-metal/albedo.png");
	Ref<Texture2D> texture2 = Texture2D::Create("assets/Textures/pbr/copper-rock/copper-rock1-alb.png");

	Random<bool> randomBool;
	Random<float> randomFloat;
	auto random = [&]() -> auto { return (randomFloat.Next() - 0.5f) * 1000.0f; };

	for (uint32_t i = 0; i < 10000; i++)
	{
		auto sprite = m_Scene->CreateEntity();

		Vec3f axis = { randomFloat.Next(), randomFloat.Next(), randomFloat.Next() };

		auto& transform = sprite.AddComponent<Transform3DComponent>();
		transform.Position = Vec3f{ random(), random(), random() };
		transform.Scale = axis * 10.f;
		transform.Rotate(randomFloat.Next() * glm::radians(360.0f), axis);

		auto& physics = sprite.AddComponent<PhysicsComponent>();
		physics.Velocity = axis;

		auto& src = sprite.AddComponent<TexturedSpriteRendererComponent>();
		src.Texture = randomBool.Next() ? texture1 : texture2;
		src.Tint = { randomFloat.Next(), randomFloat.Next(), randomFloat.Next(), 1.0f };
	}

	m_Scene->OnPlay();

	m_Font = Font::Create("assets/Fonts/arial.ttf", 48);
	m_Text.SetFont(m_Font);
	
	auto* window = GetWindow();
	m_TextCamera.SetOrthographic((float)window->GetWidth());
	m_TextCamera.Resize(window->GetWidth(), window->GetHeight());

#if 1
	/*Scene scene;
	Entity e = scene.CreateEntity();
	e.AddComponent<Transform3DComponent>();
	e.AddComponent<PhysicsComponent>();
	e.AddComponent<MeshComponent>();
	e.AddComponent<TagComponent>();
	e.AddNativeScript<NativeScriptEntity>();

	SceneSerializer serializer;
	serializer.SerializeRuntime(&scene, "defaultScene");*/

	Scene newScene;
	SceneSerializer serializer;
	serializer.DeserializeRuntime(&newScene, "defaultScene");

	newScene.GetRegistry().each([&](entt::entity e)
	{
		Entity entity = { e, &newScene };
		AllComponents{}.Each([&]<typename Component>(Component)
		{
			if (entity.HasComponent<Component>())
				QK_CORE_WARN("Entity has: {0}", typeid(Component).name());
		});
	});
#endif
}

VulkanApp::~VulkanApp()
{
	m_Scene->OnStop();
}

void VulkanApp::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(OnMouseButtonPressed));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<WindowResizedEvent>([&](WindowResizedEvent& e)
	{
		m_TextCamera.Resize(e.GetWidth(), e.GetHeight());
	});

	e.Handled = !m_ViewportSelected && e.IsInCategory(EventCategory::Input);

	if (m_Scene && !e.Handled)
		m_Scene->OnEvent(e);

	DefaultEventHandler(e);
}

void VulkanApp::OnRender()
{
	m_Scene->OnRender();
}

void VulkanApp::OnUpdate(Timestep elapsedTime)
{
	m_Scene->OnUpdate(elapsedTime);
}

void VulkanApp::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	switch (e.GetMouseButton())
	{
		case MouseCode::ButtonLeft:
		{
			m_ViewportSelected = true;
			GetWindow()->DisableCursor();
			break;
		}
		default:
			break;
	}
}

void VulkanApp::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::Escape:
		{
			m_ViewportSelected = false;
			auto* window = GetWindow();
			window->EnableCursor();
			break;
		}
		default:
			break;
	}
}
