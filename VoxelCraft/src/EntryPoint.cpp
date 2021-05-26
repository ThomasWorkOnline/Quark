#include <Quark.h>

#include "Chunk.h"
#include "ChunkRenderer.h"
#include "Player.h"
#include "PlayerController.h"
#include "World.h"
#include "Resources.h"

class VoxelCraft : public Quark::Application
{
public:
	/*Quark::Ref<Quark::Shader> m_Shader;
	Quark::BufferLayout m_Layout = {
		{ Quark::ShaderDataType::Float3, "a_Position" },
		{ Quark::ShaderDataType::Float3, "a_Normal"   },
		{ Quark::ShaderDataType::Float2, "a_TexCoord" }
	};
	Quark::Mesh m_Mesh = { m_Layout, "assets/models/arrow.obj" };

	Quark::Transform3DComponent m_Transform;*/

	void OnCreate() override
	{
		QK_TIME_SCOPE_DEBUG(VoxelCraft::OnCreate);

		GetWindow().Select();
		GetWindow().SetVSync(false);
		GetWindow().SetFullScreen(false);

		//m_Shader = Quark::Shader::Create("assets/shaders/default.glsl");

		//m_Transform.Scale = { 0.2f, 0.1f, 0.1f };
		//m_Transform.Position = { 0.0f, 65.0f, 0.0f };
	}

	void OnDestroy() override
	{
		
	}

	void OnUpdate(float elapsedTime) override
	{
		m_World.OnUpdate(elapsedTime);
		m_Controller.OnUpdate(elapsedTime);

		/*Quark::Renderer::BeginScene(m_World.GetPlayer().GetCamera().Camera.GetMatrix(),
			m_World.GetPlayer().GetTransform());

		m_Transform.Orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		Quark::Renderer::Submit(m_Shader, m_Mesh.GetVertexArray(), m_Transform);

		Quark::Renderer::EndScene();*/
	}

	void OnEvent(Quark::Event& e) override
	{
		Quark::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnKeyPressed));
		dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(VoxelCraft::OnMouseButtonPressed));

		m_World.OnEvent(e);
		m_Controller.OnEvent(e);
	}

	bool OnKeyPressed(Quark::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Quark::Key::Escape:
			GetWindow().Deselect();
			break;
		case Quark::Key::F11:
			GetWindow().SetFullScreen(!GetWindow().IsFullscreen());
			break;
		default:
			break;
		}

		return false;
	}

	bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
	{
		Ray ray;
		ray.Origin = m_World.GetPlayer().GetPosition();
		ray.Direction = glm::normalize(m_World.GetPlayer().GetTransform().GetFrontVector());

		auto collisionData = m_World.RayCast(ray.Origin, ray.Direction, 5.0f);

		switch (e.GetMouseButton())
		{
		case Quark::MouseCode::ButtonLeft:
			GetWindow().Select();

			if (collisionData.Block != Block::None && collisionData.Block != Block::Air)
			{
				m_World.ReplaceBlock(collisionData.Impact, Block::Air);
			}
			break;
		case Quark::MouseCode::ButtonRight:
		{
			if (collisionData.Block != Block::None && collisionData.Block != Block::Air)
			{
				m_World.ReplaceBlock(collisionData.Impact + (glm::vec3)GetFaceNormal(collisionData.Side), Block::Cobblestone);
			}
			break;
		}
		}
		
		return false;
	}

private:
	World m_World;
	PlayerController m_Controller = { m_World.GetPlayer() };
};

int main()
{
	auto app = new VoxelCraft();
	app->Run();
	delete app;

	return 0;
}
