#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>
#include <GLCore/Renderer/Shader.h>
#include <GLCore/Renderer/VertexArray.h>
#include <GLCore/Renderer/IndexBuffer.h>
#include <GLCore/Renderer/Renderer.h>
#include <memory>
class ExampleLayer : public GLCore::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr <VertexArray> m_VAO;
	std::unique_ptr <IndexBuffer> m_IndexBuffer;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	Renderer m_Renderer;
	GLCore::Utils::OrthographicCameraController m_CameraController;

	GLuint m_QuadVA, m_QuadVB, m_QuadIB;

	glm::vec4 m_SquareBaseColor = { 0.8f, 0.2f, 0.3f, 1.0f };
	glm::vec4 m_SquareAlternateColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	glm::vec4 m_SquareColor = m_SquareBaseColor;
};