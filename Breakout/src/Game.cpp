#include "game.h"
#include "ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>
Game::Game(GLuint width, GLuint height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete m_Renderer;
}

void Game::Init(const glm::mat4& projection)
{
	auto shader = ResourceManager::LoadShader(std::string("assets/shaders/sprite.vert.glsl"), std::string("assets/shaders/sprite.frag.glsl"), std::string(), std::string("sprite"));
	glm::mat4 projection1 = glm::ortho(0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
	shader->Bind();
	shader->SetUniform1i("image", 0);
	shader->SetUniformMat4f("projection", projection1);

	ResourceManager::LoadTexture("assets/textures/awesomeface.png", GL_TRUE, std::string("face"));
	m_Renderer = new SpriteRenderer(*shader);
}

void Game::Update(GLfloat dt)
{

}


void Game::ProcessInput(GLfloat dt)
{

}

void Game::Render()
{
	auto texture = ResourceManager::GetTexture(std::string("face"));
	m_Renderer->DrawSprite(*texture, glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), -135.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}