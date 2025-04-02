#include "game.h"
#include "ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GLCore/Core/KeyCodes.h"
#include "BallObject.h"
GameObject* Player;
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

BallObject* Ball;
Game::Game(GLuint width, GLuint height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete m_Renderer;
	delete Player;
}

void Game::Init(const glm::mat4& projection)
{
	auto shader = ResourceManager::LoadShader("assets/shaders/sprite.vert.glsl", "assets/shaders/sprite.frag.glsl", "", "sprite");
	glm::mat4 projection1 = glm::ortho(0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
	shader->Bind();
	shader->SetUniform1i("image", 0);
	shader->SetUniformMat4f("projection", projection1);
	ResourceManager::LoadTexture("assets/textures/awesomeface.png", "face");
	ResourceManager::LoadTexture("assets/textures/block.png", "block");
	ResourceManager::LoadTexture("assets/textures/block_solid.png", "block_solid");
	ResourceManager::LoadTexture("assets/textures/background.jpg","background");
	ResourceManager::LoadTexture("assets/textures/paddle.png", "paddle");


	//Level
	std::string strs[] = { "one","two","three","four" };
	for (size_t i = 0; i < 4; i++)
	{
		GameLevel one;
		one.Load((std::string("assets/levels/")+strs[i]+(".lvl")), this->Width, this->Height * 0.5);
		Levels.push_back(one);
	}
	Level = 0;

	// Configure game objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture(std::string("paddle")));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
	//Ball->Rotation =-180;
	//GameLevel two; one.Load("levels/one.lvl", this->Width, this->Height * 0.5);
	//GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height * 0.5);
	//GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height * 0.5);
	m_Renderer = new SpriteRenderer(*shader);

}

void Game::Update(GLfloat dt)
{
	Ball->Move(dt, this->Width);
	DoCollisions();

	if (Ball->Position.y >= this->Height) // 球是否接触底部边界？
	{
		this->ResetLevel();
		this->ResetPlayer();
	}
}


void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
		if (this->Keys[HZ_KEY_A])
		{
			if (Player->Position.x >= 0){
				Player->Position.x -= velocity;
				if (Ball->GetStuck())
				{
					Ball->Position.x -= velocity;
				}
			}
		}
		if (this->Keys[HZ_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->GetStuck())
					Ball->Position.x += velocity;
			}
		}
		if (this->Keys[HZ_KEY_SPACE])
			Ball->SetStuck(false);
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		//绘制背景
		m_Renderer->DrawSprite(*ResourceManager::GetTexture("background"), glm::vec2(0.0, 0.0), glm::vec2(Width, Height), 0.0f);
		this->Levels[this->Level].Draw(*m_Renderer);
		Player->Draw(*m_Renderer);
		Ball->Draw(*m_Renderer);
	}
	
	//m_Renderer->DrawSprite(*ResourceManager::GetTexture("face"), glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), -135.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Game::DoCollisions()
{
	for (GameObject& box : Levels[Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))
			{
				if (!box.IsSolid)
				{
					box.Destroyed = true;
				}
				//碰撞处理
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT)
				{
					Ball->Velocity.x = -Ball->Velocity.x;//翻转水平速度
					//重定位
					float penetration = Ball->GetRadius() - std::abs(diff_vector.x);
					if (dir == LEFT)
					{
						Ball->Position.x += penetration;//将球右移
					}
					else
					{
						Ball->Position.x -= penetration;//将球左移
					}
				}
				else
				{
					Ball->Velocity.y = -Ball->Velocity.y; // 反转垂直速度
					// 重定位
					GLfloat penetration = Ball->GetRadius() - std::abs(diff_vector.y);
					if (dir == UP)
						Ball->Position.y -= penetration; // 将球上移
					else
						Ball->Position.y += penetration; // 将球下移
				}
			}
		}
	}

	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->GetStuck() && std::get<0>(result))
	{
		// 检查碰到了挡板的哪个位置，并根据碰到哪个位置来改变速度
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ball->Position.x + Ball->GetRadius()) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);
		// 依据结果移动
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		//Ball->Velocity.y = -Ball->Velocity.y;
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
}
void Game::ResetLevel()
{
	if (this->Level == 0)this->Levels[0].Load("levels/one.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("levels/two.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("levels/three.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("levels/four.lvl", this->Width, this->Height * 0.5f);
}
void Game::ResetPlayer()
{
	// Reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}
//AABB-AABB collision
bool  Game::CheckCollision(GameObject& one, GameObject& two)
{
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		              one.Position.x + one.Size.x <= two.Position.x + two.Size.x;
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		              one.Position.y + one.Size.y <= two.Position.y + two.Size.y;
	return collisionX&&collisionY;
}

Game::Collision  Game::CheckCollision(BallObject& one, GameObject& two)
{
	glm::vec2 center(one.Position + one.GetRadius());
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y
	);
	// 获取两个中心的差矢量
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
	glm::vec2 closest = aabb_center + clamped;
	// 获得圆心center和最近点closest的矢量并判断是否 length <= radius
	difference = closest - center;
	if (glm::length(difference) <= one.GetRadius())
	{
		return std::make_tuple(true, VectorDirection(difference), difference);
	}
	else
	{
		return std::make_tuple(false, UP, glm::vec2(0, 0));
	}
}

Direction Game::VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
	  glm::vec2(0.0f, 1.0f),  // 上
	  glm::vec2(1.0f, 0.0f),  // 右
	  glm::vec2(0.0f, -1.0f), // 下
	  glm::vec2(-1.0f, 0.0f)  // 左
	};

	float max = 0.0f;
	uint32_t best_match = -1;
	for (uint32_t i = 0; i < 4; i++)
	{
		float dot_Product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_Product > max)
		{
			max = dot_Product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}
