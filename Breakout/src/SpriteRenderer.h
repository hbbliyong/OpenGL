#pragma once
#include <memory>
#include <GLCore/Renderer/Shader.h>
#include <GLCore/Renderer/VertexArray.h>
#include <GLCore/Renderer/IndexBuffer.h>
#include <GLCore/Renderer/Renderer.h>
#include <GLCore/Renderer/Texture2D.h>
#include <GLCore/Renderer/VertexBufferLayout.h>

class SpriteRenderer
{
public:
	// Constructor (inits shaders/shapes)
	SpriteRenderer(Shader& shader);
	// Destructor
	~SpriteRenderer()=default;
	// Renders a defined quad textured with given sprite
	void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
private:
	// Render state
	Shader m_Shader;
	VertexArray m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<IndexBuffer> m_IBO;
	Renderer m_Renderer;

	// Initializes and configures the quad's buffer and vertex attributes
	void initRenderData();
};
