#include "game.h"
#include "ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GLCore/Core/KeyCodes.h"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "GLCore/Core/Application.h"
#include "irrKlang/irrKlang.h"
#include "PowerUp.h"
#include "TextRenderer.h"
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
PostProcessor* Effects;
TextRenderer* Text;
GLfloat   ShakeTime = 0.0f;

irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
Game::Game(GLuint width, GLuint height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete m_Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	SoundEngine->drop();
}

void Game::Init(const glm::mat4& projection1)
{
	auto shader = ResourceManager::LoadShader("assets/shaders/sprite.vert.glsl", "assets/shaders/sprite.frag.glsl", "", "sprite");
	auto particleShader = ResourceManager::LoadShader("assets/shaders/particle.vert.glsl", "assets/shaders/particle.frag.glsl", "", "particle");
	ResourceManager::LoadShader("assets/shaders/post_processing.vert.glsl", "assets/shaders/post_processing.frag.glsl", "", "postprocessing");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
	shader->Bind();
	shader->SetUniform1i("image", 0);
	shader->SetUniformMat4f("projection", projection);

	particleShader->Bind();
	particleShader->SetUniform1i("sprite", 0);
	particleShader->SetUniformMat4f("projection", projection);
	//ResourceManager::LoadTexture("assets/textures/awesomeface.png", "face");
	ResourceManager::LoadTexture("assets/textures/awesomeface.png", "face");
	ResourceManager::LoadTexture("assets/textures/block.png", "block");
	ResourceManager::LoadTexture("assets/textures/block_solid.png", "block_solid");
	ResourceManager::LoadTexture("assets/textures/background.jpg", "background");
	ResourceManager::LoadTexture("assets/textures/paddle.png", "paddle");
	ResourceManager::LoadTexture("assets/textures/particle.png", "particle");
	auto  filenames = {
		"powerup_speed",
		"powerup_sticky",
		"powerup_increase",
		"powerup_confuse",
		"powerup_chaos",
		"powerup_passthrough"
	};
	for (std::string filename : filenames)
	{
		ResourceManager::LoadTexture("assets/textures/" + filename + ".png", filename);
	}
	//Level
	std::string strs[] = { "one","two","three","four" };
	for (size_t i = 0; i < 4; i++)
	{
		GameLevel one;
		one.Load((std::string("assets/levels/") + strs[i] + (".lvl")), this->Width, this->Height * 0.5);
		Levels.push_back(one);
	}
	Level = 0;

	// Configure game objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture(std::string("paddle")));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
	Particles = new ParticleGenerator(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		500);
	m_Renderer = new SpriteRenderer(*shader);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
	//Effects->Confuse = true;
	//Effects->Chaos = true;
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("assets/fonts/ARIALUNI.TTF", 24);
	SoundEngine->play2D("assets/audio/breakout.mp3", true);
}

void Game::Update(GLfloat dt)
{
	Ball->Move(dt, this->Width);
	DoCollisions();
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->GetRadius() / 2.0f));

	UpdatePowerUps(dt);

	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
		{
			Effects->Shake = false;
		}
	}
	if (Ball->Position.y >= this->Height) // 球是否接触底部边界？
	{
		// 玩家是否已失去所有生命值? : 游戏结束
		--this->Lives;
		if (Lives == 0)
		{
			this->ResetLevel();
			State = GAME_MENU;
		}
		this->ResetPlayer();
	}

	if (State==GAME_ACTIVE&&Levels[Level].IsCompleted())
	{
		ResetLevel();
		ResetPlayer();
		Effects->Chaos = true;
		State = GAME_WIN;
	}
}


void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
		if (this->Keys[HZ_KEY_A] || this->Keys[HZ_KEY_LEFT])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
				if (Ball->GetStuck())
				{
					Ball->Position.x -= velocity;
				}
			}
		}
		if (this->Keys[HZ_KEY_D] || this->Keys[HZ_KEY_RIGHT])
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
	else if (State == GAME_MENU)
	{
		if (Keys[HZ_KEY_ENTER]&&!KeysProcessed[HZ_KEY_ENTER])
		{
			State = GAME_ACTIVE;
			KeysProcessed[HZ_KEY_ENTER] = true;
		}
		if (Keys[HZ_KEY_W]&&!KeysProcessed[HZ_KEY_W])
		{
			Level = (Level + 1) % 4;
			KeysProcessed[HZ_KEY_W] = true;
		}
		if (Keys[HZ_KEY_S]&&!KeysProcessed[HZ_KEY_S])
		{
			if (Level > 0)
				--Level;
			else
				Level = 3;
			KeysProcessed[HZ_KEY_S] = true;
		}
	}
	if (this->State == GAME_WIN)
	{
		if (this->Keys[HZ_KEY_ENTER])
		{
			this->KeysProcessed[HZ_KEY_ENTER] = GL_TRUE;
			Effects->Chaos = GL_FALSE;
			this->State = GAME_MENU;
		}
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Effects->BeginRender();
		//绘制背景
		m_Renderer->DrawSprite(*ResourceManager::GetTexture("background"), glm::vec2(0.0, 0.0), glm::vec2(Width, Height), 0.0f);
		this->Levels[this->Level].Draw(*m_Renderer);
		Player->Draw(*m_Renderer);
		// Draw PowerUps
		for (auto& powerUp : PowerUps)
		{
			if (!powerUp.Destroyed)
			{
				powerUp.Draw(*m_Renderer);
			}
		}
		Particles->Draw();
		Ball->Draw(*m_Renderer);
		Effects->EndRender();
		Effects->Render(GLCore::Application::Get().GetWindow().GetTime());

		std::stringstream ss; ss << this->Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (State==GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", 550.0f, Height / 2-40.0f, 1.0f);
		Text->RenderText("Press W or S to select level", 545.0f, Height / 2 , 0.75f);
		Text->RenderText("Current  Level: "+std::to_string(Level+1), Width/2-120, Height / 2 + 40.f, 2.0f,glm::vec3(0.0,1.0,0.0));
	}
	if (State==GAME_WIN)
	{
		Text->RenderText("You WON!!!", 450, Height / 2 - 20, 1.0, glm::vec3(0.0, 1.0, 0.0));
		Text->RenderText("Press Enter to retry or ESC to quit", 250, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));
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
					SpawnPowerUps(box);
					SoundEngine->play2D("assets/audio/bleep.mp3", false);
				}
				else
				{
					ShakeTime = 0.05f;
					Effects->Shake = true;
					SoundEngine->play2D("assets/audio/solid.wav", false);
				}
				//碰撞处理
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->PassThrough && !box.IsSolid))
				{
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
		Ball->SetStuck(Ball->Sticky);
		SoundEngine->play2D("assets/audio/bleep.wav", false);
	}

	for (PowerUp& powerUp : PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= Height)
			{
				powerUp.Destroyed = true;
			}
			if (CheckCollision(*Player, powerUp))
			{
				//道具和挡板碰撞，激活
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;;
				powerUp.Activated = true;
				SoundEngine->play2D("assets/audio/powerup.wav", false);
			}
		}
	}
}
void Game::ResetLevel()
{
	if (this->Level == 0)this->Levels[0].Load("assets/levels/one.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("assets/levels/two.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("assets/levels/three.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("assets/levels/four.lvl", this->Width, this->Height * 0.5f);
	this->Lives = 3;
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
	return collisionX && collisionY;
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
bool ShouldSpawn(GLuint change)
{
	GLuint random = rand() % change;
	return random == 0;
}
void Game::SpawnPowerUps(GameObject& block)
{
	if (ShouldSpawn(75))//1/75的几率
		PowerUps.emplace_back("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed"));
	if (ShouldSpawn(75))
		PowerUps.emplace_back("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky"));
	if (ShouldSpawn(75))
		PowerUps.emplace_back("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough"));
	if (ShouldSpawn(75))
		PowerUps.emplace_back("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase"));
	if (ShouldSpawn(15))
		PowerUps.emplace_back("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse"));
	if (ShouldSpawn(15))
		PowerUps.emplace_back("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos"));

}

void Game::UpdatePowerUps(float dt)
{
	for (PowerUp& powerUp : PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;
			if (powerUp.Duration <= 0.0f)
			{
				powerUp.Activated = false;
				//停用效果
				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(PowerUps, "sticky"))
					{
						// 仅当没有其他sticky效果处于激活状态时重置，以下同理
						Ball->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(PowerUps, "pass-through"))
					{
						Ball->PassThrough = false;
						Ball->Color = glm::vec3(1.0);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(PowerUps, "confuse"))
					{
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(PowerUps, "chaos"))
					{
						Effects->Chaos = false;
					}
				}
			}
		}
	}
	PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
		[](const PowerUp& powerUp)
		{
			return powerUp.Destroyed && !powerUp.Activated;
		}), PowerUps.end());
}

void Game::ActivatePowerUp(PowerUp& powerUp)
{
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = true; // 只在chaos未激活时生效，chaos同理
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
}

bool Game::IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
	for (const auto& powerUp : powerUps)
	{
		if (powerUp.Activated)
		{
			if (powerUp.Type == type)
			{
				return true;
			}
		}
	}
	return false;
}
