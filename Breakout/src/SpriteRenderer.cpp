#include "SpriteRenderer.h"
#include "glm/gtc/matrix_transform.hpp"
SpriteRenderer::SpriteRenderer(Shader& shader)
	:m_Shader(shader)
{
	initRenderData();
}

void SpriteRenderer::DrawSprite(Texture2D& texture, glm::vec2 positon, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
    Renderer renderer;
    m_Shader.Bind();
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(positon, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f));
    m_Shader.SetUniformMat4f("model", model);
    m_Shader.SetUniform3fv("spriteColor", color);
    texture.Bind();
    renderer.Draw(m_VAO, *m_IBO, m_Shader);
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
    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
    };
    unsigned int indexs[] = {
        0,1,2,1,3,2
    };

    m_VBO=std::make_unique<VertexBuffer>(vertices,sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    m_VAO.AdddBuffer(*m_VBO, layout);

    m_IBO = std::make_unique<IndexBuffer>(indexs, 6);
}
