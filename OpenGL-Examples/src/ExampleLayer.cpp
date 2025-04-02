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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	m_Shader = std::make_unique<Shader>(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);
	m_Shader->Bind();
	//glCreateVertexArrays(1, &m_QuadVA);
	//glBindVertexArray(m_QuadVA);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,0.0f,0.0f,
		 0.5f, -0.5f, 0.0f,1.0f,0.0f,
		 0.5f,  0.5f, 0.0f,1.0f,1.0f,
		-0.5f,  0.5f, 0.0f,0.0f,1.0f
	};

	m_VAO = std::make_unique<VertexArray>();
	m_VertexBuffer = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
	VertexBufferLayout vbl;
	vbl.Push<float>(3);
	vbl.Push<float>(2);
	m_VAO->AdddBuffer(*m_VertexBuffer, vbl);
	//glCreateBuffers(1, &m_QuadVB);
	//glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
	//glCreateBuffers(1, &m_QuadIB);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

	m_Texture = std::make_unique<Texture2D>("E:/study/OpenGL/Breakout/assets/textures/awesomeface.png");
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
}

void ExampleLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_Texture->Bind();

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 model1 = glm::translate(model, glm::vec3(1, 0, 0));
	m_Shader->SetUniformMat4f("u_Model", model1);
	m_Shader->SetUniform4fv("u_Color", m_SquareColor);


	m_Shader->SetUniform1i("u_Texture", 0);

	//m_Renderer.Clear();
	m_Renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);

	glm::mat4 model2 = glm::translate(model, glm::vec3(-1, 0, 0));

	m_Shader->SetUniform4fv("u_Color", glm::vec4(0, 1, 0, 1));
	m_Shader->SetUniformMat4f("u_Model", model2);
	m_Renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
		m_SquareColor = m_SquareBaseColor;
	ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));
	ImGui::End();
}
