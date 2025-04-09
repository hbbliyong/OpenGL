#pragma once
#include<GLCore/Renderer/Texture2D.h>

const glm::vec2 SIZE1(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);
class PowerUp :public GameObject
{

public:
	std::string Type;
	float Duration;
	bool Activated;

	PowerUp(const std::string& type, glm::vec3 color, float duration,
		glm::vec2 position, std::shared_ptr<Texture2D> texture) :
		GameObject(position, SIZE1, texture, color, VELOCITY),
		Type(type), Duration(duration), Activated(false)
	{
	}
};