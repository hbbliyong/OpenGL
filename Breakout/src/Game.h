#pragma once
#include <glad/glad.h>
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include<vector>
class BallObject;
// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

class Game
{
public:
	// 游戏状态
	GameState  State;
	GLboolean  Keys[1024];
	GLuint     Width, Height;
	typedef std::tuple<bool, Direction, glm::vec2> Collision;
public:
	// 构造函数/析构函数
	Game(GLuint width, GLuint height);
	~Game();
	// 初始化游戏状态（加载所有的着色器/纹理/关卡）
	void Init(const glm::mat4& projection);
	// 游戏循环
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
	void DoCollisions();

	// Reset
	void ResetLevel();
	void ResetPlayer();

private:
	bool  CheckCollision(GameObject& one, GameObject& two);
	Collision  CheckCollision(BallObject& one, GameObject& two);

	Direction VectorDirection(glm::vec2 target);


private:
	SpriteRenderer* m_Renderer;
	std::vector<GameLevel> Levels;
	uint32_t Level;
};