#include "ExampleLayer.h"
#include "GLCore/Renderer/VertexBufferLayout.h"
using namespace GLCore;
using namespace GLCore::Utils;

ExampleLayer::ExampleLayer()
	: m_CameraController(16.0f / 9.0f)
{

}

ExampleLayer::~ExampleLayer()
{

}

void ExampleLayer::OnAttach()
{
	EnableGLDebugging();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto &win = Application::Get().GetWindow();
	m_Breakout = std::make_unique<Game>(win.GetWidth(), win.GetHeight());
	//m_Breakout = std::make_unique<Game>(800, 600);
	m_Breakout->Init(m_CameraController.GetCamera().GetViewProjectionMatrix());
}

void ExampleLayer::OnDetach()
{
	//glDeleteVertexArrays(1, &m_QuadVA);
	//glDeleteBuffers(1, &m_QuadVB);
	//glDeleteBuffers(1, &m_QuadIB);
	//m_VertexArray->UnBind();
	//m_VertexArray
}

void ExampleLayer::OnEvent(Event& event)
{
	m_CameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressedEvent>(
		[&](MouseButtonPressedEvent& e)
		{
			m_SquareColor = m_SquareAlternateColor;
			return false;
		});
	dispatcher.Dispatch<MouseButtonReleasedEvent>(
		[&](MouseButtonReleasedEvent& e)
		{
			m_SquareColor = m_SquareBaseColor;
			return false;
		});
	dispatcher.Dispatch< KeyPressedEvent>(
		[&](KeyPressedEvent& e)
		{
			auto key = e.GetKeyCode();
			if (key == 1 || key == 2)
			{
				//Application::Get().GetWindow().
				//close
			}
			if (key > 0 && key < 1024)
			{
				m_Breakout->Keys[key] = GL_TRUE;

			}

			return false;
		});
	dispatcher.Dispatch< KeyReleasedEvent>(
		[&](KeyReleasedEvent& e)
		{
			auto key = e.GetKeyCode();
			if (key == 1 || key == 2)
			{
				//Application::Get()
				//close
			}
			if (key > 0 && key < 1024)
			{
				m_Breakout->Keys[key] = GL_FALSE;
				m_Breakout->KeysProcessed[key] = GL_FALSE;
			}
			return false;
		});

}

void ExampleLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	auto x = m_CameraController.GetCamera().GetViewProjectionMatrix();
	m_Breakout->ProcessInput(ts);
	m_Breakout->Update(ts);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_Breakout->Render();
}

void ExampleLayer::OnImGuiRender()
{
	//ImGui::Begin("Controls");
	//if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
	//	m_SquareColor = m_SquareBaseColor;
	//ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));
	//ImGui::End();
}
