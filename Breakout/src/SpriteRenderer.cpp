#include "SpriteRenderer.h"
#include "glm/gtc/matrix_transform.hpp"
SpriteRenderer::SpriteRenderer(Shader& shader)
	:m_Shader(shader)
{
	initRenderData();
}

void SpriteRenderer::DrawSprite(Texture2D& texture, glm::vec2 positon, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{  
    m_Shader.Bind();
    glm::mat4 model= glm::mat4(1.0f);;
    //glViewport(0, 0, 800, 600);
    model = glm::translate(model, glm::vec3(positon, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f));
    m_Shader.SetUniformMat4f("model", model);
    m_Shader.SetUniform3fv("spriteColor", color);
   // m_Shader.SetUniform1i("image", 0);
    texture.Bind();

    GLint currentVBO;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVBO);
    m_Renderer.Draw(m_VAO, *m_IBO, m_Shader);
}

void SpriteRenderer::initRenderData()
{
    //GLfloat vertices[] = {
    //    // Pos      // Tex
    //    0.0f, 1.0f, 0.0f, 1.0f,
    //    1.0f, 0.0f, 1.0f, 0.0f,
    //    0.0f, 0.0f, 0.0f, 0.0f,

    //    0.0f, 1.0f, 0.0f, 1.0f,
    //    1.0f, 1.0f, 1.0f, 1.0f,
    //    1.0f, 0.0f, 1.0f, 0.0f
    //};
    //GLfloat vertices[] = {
    //    // Pos      // Tex
    //     0.5f,  0.5f,    1.0f, 1.0f, // top right
    //     0.5f, -0.5f,    1.0f, 0.0f, // bottom right
    //    -0.5f, -0.5f,    0.0f, 0.0f, // bottom left
    //    -0.5f,  0.5f,    0.0f, 1.0f  // top left 
    //};
	//float vertices[] = {
	//	-0.5f, -0.5f,0.0f,0.0f,
	//	 0.5f, -0.5f,1.0f,0.0f,
	//	 0.5f,  0.5f,1.0f,1.0f,
	//	-0.5f,  0.5f,0.0f,1.0f
	//};
 //   uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

// 去重后的唯一顶点数组（VBO）
	float vertices[] = {
		0.0f, 1.0f, 0.0f, 1.0f,  // 索引0
		1.0f, 0.0f, 1.0f, 0.0f,  // 索引1
		0.0f, 0.0f, 0.0f, 0.0f,  // 索引2
		1.0f, 1.0f, 1.0f, 1.0f   // 索引3
	};

	// 索引数组（IBO）
	unsigned int indices[] = {
		0, 1, 2,   // 第一个三角形（原始顶点0,1,2）
		0, 3, 1    // 第二个三角形（原始顶点3,4,5）
	};

    m_VBO=std::make_unique<VertexBuffer>(vertices,sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    m_VAO.AdddBuffer(*m_VBO, layout);

    m_IBO = std::make_unique<IndexBuffer>(indices, 6);
}
