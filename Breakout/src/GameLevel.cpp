#include "GameLevel.h"
#include "ResourceManager.h"

#include <fstream>
#include <sstream>
void GameLevel::Load(const std::string& file, uint32_t levelWidth, uint32_t levelHeight)
{
	Bricks.clear();
	uint32_t tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<uint32_t>> tileData;

	if (fstream)
	{
		while (std::getline(fstream, line))
		{
			std::istringstream sstream(line);
			std::vector<uint32_t> row;
			while (sstream >> tileCode)// Read each word seperated by spaces
			{
				row.push_back(tileCode);
			}
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
		{
			this->init(tileData, levelWidth, levelHeight);
		}
	}
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
	for (GameObject& tile : Bricks)
	{
		if (!tile.Destroyed)
			tile.Draw(renderer);
	}
}

bool GameLevel::IsCompleted()
{
	for (auto& tile : this->Bricks)
	{
		if (!tile.IsSolid && !tile.Destroyed)
		{
			return false;
		}
	}
	return true;
}

void GameLevel::init(std::vector < std::vector<uint32_t>> tileData, uint32_t levelWidth, uint32_t levelHeight)
{
	uint32_t height = tileData.size();
	uint32_t width = tileData[0].size();
	float unit_width = levelWidth / width, unit_height = levelHeight / height;

	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			if (tileData[y][x] == 1)//Solid
			{
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture(std::string("block_solid")), glm::vec3(0.8f, .8f, .7f));
				obj.IsSolid = true;
				Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1)// Non-solid; now determine its color based on level data
			{
				glm::vec3 color = glm::vec3(1.0f); // original: white
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture(std::string("block")), color));
			}
		}
	}
}

