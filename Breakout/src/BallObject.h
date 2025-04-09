#pragma once
#include "GameObject.h"
class BallObject :public GameObject
{
public:
	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, const std::shared_ptr<Texture2D>& sprite);

	glm::vec2 Move(float dt, uint32_t window_width);
	void Reset(glm::vec2 position, glm::vec2 velocity);

	inline	bool GetStuck() { return m_Stuck; }
	inline	bool SetStuck(bool stuck) { return m_Stuck = stuck; }
	inline	float GetRadius() { return m_Radius; }
public:
	bool Sticky = false;
	bool PassThrough = false;
private:
	float m_Radius;
	bool m_Stuck;
};