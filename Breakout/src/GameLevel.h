#pragma once
#include "GameObject.h"
class GameLevel
{
public:
	GameLevel() = default;
	void Load(const std::string& file, uint32_t levelWidth, uint32_t levelHeight);
	void Draw(SpriteRenderer& renderer);
	bool IsCompleted();
public:
	std::vector<GameObject> Bricks;

private:
	void init(std::vector < std::vector<uint32_t>> tileData, uint32_t levelWidth, uint32_t levelHeight);
};