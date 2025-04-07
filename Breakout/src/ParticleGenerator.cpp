#include "ParticleGenerator.h"
ParticleGenerator::ParticleGenerator(std::shared_ptr<Shader>& shader, std::shared_ptr<Texture2D>& texture2D, uint32_t amount) :
	m_Shader(shader), m_Texture(texture2D), m_Amount(amount)
{
	this->init();
}

void ParticleGenerator::Update(float dt, GameObject& object, uint32_t newParticles, glm::vec2 offset /*= glm::vec2(0.0f, 0.0f)*/)
{
	for (uint32_t i=0;i<newParticles;i++)
	{
		int unusedParticle = firstUnuseParticle();
		respawnParticle(m_Particles[unusedParticle], object, offset);
	}

	for (uint32_t i=0;i<m_Amount;i++)
	{
		Particle& p = m_Particles[i];
		p.Life -= dt;//reduce life
		if (p.Life>0.0f)
		{
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 2.5;
		}
	}
}

void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_Shader->Bind();
	for (Particle& particle:m_Particles)
	{
		if (particle.Life > 0.0f)
		{
			m_Shader->SetUniform2f("offset", particle.Position);
			m_Shader->SetUniform4fv("color", particle.Color);
			m_Texture->Bind();
			glBindVertexArray(m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	//Reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->m_VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->m_VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	for (size_t i = 0; i < m_Amount; i++)
	{
		m_Particles.emplace_back();
	}
}
uint32_t lastUsedParticle = 0;
uint32_t ParticleGenerator::firstUnuseParticle()
{
	for (uint32_t i = lastUsedParticle; i < m_Amount; ++i)
	{
		if (m_Particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	for (uint32_t i = 0; i < m_Amount; ++i)
	{
		if (m_Particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset /*= glm::vec2(0.0f, 0.0f)*/)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor,1.0);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}

