#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <GLCore/Renderer/Shader.h>
#include <GLCore/Renderer/VertexArray.h>
#include <GLCore/Renderer/IndexBuffer.h>
#include <GLCore/Renderer/Renderer.h>
#include <GLCore/Renderer/Texture2D.h>
#include <GLCore/Renderer/VertexBufferLayout.h>
#include "GameObject.h"
struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float Life;

	Particle():Position(0.0),Velocity(0.0f),Color(1.0f),Life(0.0){}
};

class ParticleGenerator
{
public:
	ParticleGenerator(std::shared_ptr<Shader>& shader, std::shared_ptr<Texture2D>& texture2D, uint32_t amount);
	void Update(float dt, GameObject& object, uint32_t newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	void Draw();
private:
	void init();
	uint32_t firstUnuseParticle();
	void  respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
private:
	std::vector<Particle> m_Particles;
	uint32_t m_Amount;
	std::shared_ptr<Texture2D> m_Texture;
	std::shared_ptr<Shader> m_Shader;
	unsigned int m_VAO;
};