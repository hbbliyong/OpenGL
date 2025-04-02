#include "BallObject.h"

BallObject::BallObject():GameObject(),m_Radius(12.5f),m_Stuck(true)
{
}

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, const std::shared_ptr<Texture2D>& sprite)
	:GameObject(pos,glm::vec2(radius*2,radius*2),sprite,glm::vec3(1.0),velocity),m_Radius(radius),m_Stuck(true)
{
}

glm::vec2 BallObject::Move(float dt, uint32_t window_width)
{
	// If not stuck to player board
	if (!m_Stuck)
	{
		Position += Velocity * dt;
		// Then check if outside window bounds and if so, reverse velocity and restore at correct position
		if (Position.x <= 0.0)
		{
			Velocity.x = -Velocity.x;
			Position.x =0.0f;
		}
		else if (Position.x + Size.x >= window_width)
		{
			Velocity.x = -Velocity.x;
			Position.x = window_width-Size.x;
		}
		if (this->Position.y <= 0.0f)
		{
			this->Velocity.y = -this->Velocity.y;
			this->Position.y = 0.0f;
		}
	}
	return Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->m_Stuck = true;
}
