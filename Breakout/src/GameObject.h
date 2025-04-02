#pragma once
#include <glm/glm.hpp>
#include "GLCore/Renderer/Texture2D.h"
#include "SpriteRenderer.h"
class GameObject
{
public:
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, const std::shared_ptr<Texture2D>& sprite, glm::vec3 color = glm::vec3(1.0), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void Draw(SpriteRenderer& renderer);
public:
	glm::vec2 Position, Size, Velocity;
	glm::vec3 Color;
	float Rotation;
	bool IsSolid;
	bool Destroyed;
	std::shared_ptr<Texture2D> Sprite;
};