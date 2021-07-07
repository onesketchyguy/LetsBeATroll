#pragma once

std::wstring GetPlayerSprite(int no)
{
	std::wstring map;
	switch (no)
	{
	case 0:
		// Default Space ship
		map = L"....444444....33333.......4424.......442444.....4444414444....331113333...331113333.4444414444...442444.......4424........33333.........444444..";
		break;
	case 1:
		// Explosion
		map = L"...........##....#....#..#...#........#.............#..#.......#....#......#......####.....#.........##.......#.....#....#.......#..............";
		break;
	default:
		break;
	}

	return map;
}

std::wstring GetPersonSprite(int no)
{
	std::wstring map;
	switch (no)
	{
	case 0:
		// UFO
		map = L"..........................########...#.#..#.#.#.########################.#.#.#..#.#...########..................................................";
		break;
	case 1:
		// Explosion
		map = L"...........##....#....#..#...#........#.............#..#.......#....#......#......####.....#.........##.......#.....#....#.......#..............";
		break;
	default:
		break;
	}

	return map;
}

class Projectile
{
public:
	int side = 0;

	int x, y = 0;
	int size = 0;
	float moveX = 0, moveY = 0;
	float angleX, angleY = 0;

	void Update()
	{
		moveX += angleX;
		moveY += angleY;

		if (moveY > 1)
		{
			y++;
			moveY--;

			if (moveY > 0 && moveY < 1) {
				moveY = 0;
			}
		}

		if (moveY < -1)
		{
			y--;
			moveY++;

			if (moveY > -1 && moveY < 0) {
				moveY = 0;
			}
		}

		if (moveX > 1)
		{
			x++;
			moveX--;

			if (moveX > 0 && moveX < 1) {
				moveX = 0;
			}
		}

		if (moveX < -1)
		{
			x--;
			moveX++;

			if (moveX > -1 && moveX < 0) {
				moveX = 0;
			}
		}
	}
};

class Collider {
public:
	Collider() = default;

public:
	int width, height;
	int offX, offY;

	int posX, posY;

public:
	int GetTop() {
		return posY + offY;
	}

	int GetBottom() {
		return posY + offY + height;
	}

	int GetMiddleY() {
		return (int)(posY + offY + (height / 2.0f));
	}

	int GetLeft() {
		return posX + offX;
	}

	int GetRight() {
		return posX + offX + width;
	}

	int GetMiddleX() {
		return (int)(posX + offX + (width / 2.0f));
	}

public:
	void UpdatePosition(int x, int y)
	{
		posX = x;
		posY = y;
	}

	bool CheckOverlaps(int x, int y) {
		bool withinY = y <= GetBottom() && y >= GetTop();
		bool withinX = x >= GetLeft() && x <= GetRight();

		return withinX && withinY;
	}
};

class Entity
{
public:
	Collider* collider;

	int side = 0;

	olcSprite* sprite;
	int width, height;

	bool isDead = false;
	float deathTimer = 0;

	int x, y;

	float moveX = 0, moveY = 0;

	void BaseUpdate()
	{
		if (moveY > 1)
		{
			y++;
			moveY--;

			if (moveY > 0 && moveY < 1) {
				moveY = 0;
			}
		}

		if (moveY < -1)
		{
			y--;
			moveY++;

			if (moveY > -1 && moveY < 0) {
				moveY = 0;
			}
		}

		if (moveX > 1)
		{
			x++;
			moveX--;

			if (moveX > 0 && moveX < 1) {
				moveX = 0;
			}
		}

		if (moveX < -1)
		{
			x--;
			moveX++;

			if (moveX > -1 && moveX < 0) {
				moveX = 0;
			}
		}

		collider->UpdatePosition(x, y);
	}

	virtual void Update() {
		BaseUpdate();
	}

	void RandomizePosition(int xRangeMin, int xRangeMax, int yRangeMin, int yRangeMax)
	{
		if (xRangeMax - xRangeMin != 0) {
			x = xRangeMin + (rand() % (xRangeMax - xRangeMin));
		}

		if (yRangeMax - yRangeMin != 0) {
			y = yRangeMin + (rand() % (yRangeMax - yRangeMin));
		}
	}

	bool CheckOverlaps(Projectile* projectile)
	{
		if (isDead || projectile->side == side) return false;

		return collider->CheckOverlaps(projectile->x, projectile->y);
	}

	void SetSprite(std::wstring data, bool regenerateCollider = false)
	{
		int len = width * height;

		if (regenerateCollider == true)
		{
			// Calculate the sprite and the collider at the same time
			collider = new Collider();

			collider->width = -1;
			collider->height = -1;

			collider->offX = width;
			collider->offY = height;

			for (size_t i = 0; i < len; i++)
			{
				int ix = i % width;
				int iy = i / width;

				if (data[i] != '.')
				{
					// Calculate the start position
					if (ix < collider->offX)
					{
						collider->offX = ix;
					}

					if (iy < collider->offY)
					{
						collider->offY = iy;
					}

					// Calculate the bounds
					if (ix - collider->offX > collider->width)
					{
						collider->width = ix - collider->offX;
					}

					if (iy - collider->offY > collider->height)
					{
						collider->height = iy - collider->offY;
					}

					// Set the pixel
					sprite->SetGlyph(ix, iy, data[i]); // NOTE: we don't have to set the pixel when there is none since the default pixel is black and empty
					sprite->SetColour(ix, iy, FG_WHITE);
				}
			}
		}
		else
		{
			// Calculate the sprite
			for (size_t i = 0; i < len; i++)
			{
				int x = i % width;
				int y = i / width;

				sprite->SetGlyph(x, y, data[i]);
				sprite->SetColour(x, y, data[i] != '.' ? FG_WHITE : FG_BLACK);
			}
		}
	}

public:
	Entity(int width, int height, int x, int y) {
		this->x = x;
		this->y = y;

		moveX = moveY = 0;

		this->width = width;
		this->height = height;

		sprite = new olcSprite(width, height);
	};

	Entity() = default;
};

class Enemy : public Entity {
private:
	int tokenCount = 0;
public:
	bool moveDown = false;

	int screenHeight = 0;

	float shootDesire = 1.0f;

	bool HasToken() {
		return tokenCount > 0;
	}

	void GiveToken() {
		tokenCount++;
	}

	void TakeToken() {
		tokenCount--;
	}

	void UpdateMove() {
		if (collider->GetBottom() >= screenHeight) {
			moveDown = false;
		}

		if (collider->GetTop() <= 0) {
			moveDown = true;
		}

		moveY += moveDown ? 0.1f : -0.1f;
	}

	bool WantsToShoot(float elapsedTime) {
		shootDesire += (rand() % 100) * elapsedTime;

		// Loop around 3 never allow this number to read more than 300 %
		if (shootDesire >= 3) shootDesire = 0;

		return shootDesire >= 1.0f;
	}

	virtual void Update() {
		BaseUpdate();
		UpdateMove();
	}

public:
	Enemy(int width, int height, int x, int y, int screenHeight) {
		this->x = x;
		this->y = y;

		moveX = moveY = 0;
		isDead = false;

		this->width = width;
		this->height = height;
		this->screenHeight = screenHeight;

		sprite = new olcSprite(width, height);
	};

	Enemy() = default;
};

class Particle {
public:
	int x, y = 0;

	float moveX, moveY;
	float velX, velY;

public:
	void UpdatePosition(float elapsedTime)
	{
		moveX += velX * elapsedTime;
		moveY += velY * elapsedTime;

		if (moveY > 1)
		{
			y++;
			moveY--;

			if (moveY > 0 && moveY < 1) {
				moveY = 0;
			}
		}

		if (moveY < -1)
		{
			y--;
			moveY++;

			if (moveY > -1 && moveY < 0) {
				moveY = 0;
			}
		}

		if (moveX > 1)
		{
			x++;
			moveX--;

			if (moveX > 0 && moveX < 1) {
				moveX = 0;
			}
		}

		if (moveX < -1)
		{
			x--;
			moveX++;

			if (moveX > -1 && moveX < 0) {
				moveX = 0;
			}
		}
	}

public:
	Particle() = default;

	Particle(int x, int y, PIXEL_TYPE shade) {
		this->x = x;
		this->y = y;
		//this->shade = shade;
	}
};

class StarSystem
{
private:
	Particle* particles;
	olcConsoleGameEngine* renderer;
public:
	int count = 100;
	float speed = 30.0f;

	void InitParticle(int particleIndex)
	{
		particles[particleIndex].velX = -((rand() % 10) + 1) * speed;
		particles[particleIndex].velY = (rand() % 2) - 0.5f;

		particles[particleIndex].x = renderer->screenWidth - 1;
		particles[particleIndex].y = rand() % renderer->screenHeight;
	}

public:
	void DrawField(float elapsedTime)
	{
		for (size_t i = 0; i < count; i++)
		{
			if (renderer->frameCount % 2 == 0) {
				float moveAmount = speed * elapsedTime;

				bool twinkle = rand() % 10 > 6;

				particles[i].UpdatePosition(elapsedTime);

				if (particles[i].x <= 0) {
					InitParticle(i);
				}

				if (particles[i].y <= 0) {
					InitParticle(i);
				}

				if (particles[i].y >= renderer->screenHeight) {
					InitParticle(i);
				}
			}

			renderer->Draw(particles[i].x, particles[i].y, '*');
		}
	}

public:
	StarSystem(olcConsoleGameEngine* renderer, int count) {
		this->renderer = renderer;
		this->count = count;

		particles = new Particle[count];
		for (size_t i = 0; i < count; i++)
		{
			particles[i].x = rand() % renderer->screenWidth;
			particles[i].y = rand() % renderer->screenHeight;

			particles[i].velX = -((rand() % 10) + 1) * speed;
			particles[i].velY = (rand() % 2) - 0.5f;

			particles[i].moveX = particles[i].moveY = 0;
		}
	}
};

class SideScroller : public olcConsoleGameEngine
{
private:
	enum Screen {
		Game,
		MainMenu,
		GameOver
	};

	Screen currentScreen;

	StarSystem* starField;

	Entity* player;
	const float playerSpeed = 55.0f;

	Enemy* enemies;
	const int ENEMY_COUNT = 5;

	std::vector<Entity*> entities;

	Projectile* projectiles;
	int currentProjectile;
	const int PROJECTILE_COUNT = 50;

	float lastShot = 0;
	const float SHOT_TIMER = 0.9f;

	int SHOOT_TOKENS = 1;

	int score = 0;
	int lives = 3;

	float gameOverTime = 0;

	void PlaySpawnDidi(float pitch)
	{
		PlayBeep(pitch * 300, 50.0f);
		PlayBeep(pitch * 600, 50.0f);
		PlayBeep(pitch * 900, 100.0f);
	}

	void PlayDeathDidi()
	{
		PlayBeep(300, 100.0f);
		PlayBeep(200, 100.0f);
		PlayBeep(150, 300.0f);
	}

	void PlayGameOver()
	{
		PlayBeep(800, 100.0f);
		PlayBeep(500, 100.0f);
		PlayBeep(300, 100.0f);
		PlayBeep(200, 100.0f);
		PlayBeep(150, 500.0f);
	}

	void DrawEntity(Entity* entity)
	{
		for (size_t w = 0; w < entity->width; w++)
		{
			for (size_t h = 0; h < entity->height; h++)
			{
				short color = entity->sprite->GetColour(w, h);

				if (color != FG_BLACK)
				{
					short glyph = entity->sprite->GetGlyph(w, h);

					SetPixel(entity->x + w, entity->y + h, GetPixelFromChar(glyph));
				}
			}
		}
	}

	void UpdateEntity(Entity* entity, float deltaTime)
	{
		if (entity->isDead == true) {
			if (entity->deathTimer == 0) {
				entity->deathTimer = deltaTime;

				if (entity->side == player->side)
				{
					entity->SetSprite(GetPlayerSprite(1));
					lives--;

					if (lives > 0) {
						PlayDeathDidi();
					}
					else {
						PlayGameOver();
					}
				}
				else
				{
					entity->SetSprite(GetPersonSprite(1));
					PlayBeep(300, 50.0f);

					score += 10;
					if (score % 500 == 0)
					{
						lives++;
						PlaySpawnDidi(2);
					}
				}

				//PlaySample(explosionSample);
			}
			else if (entity->deathTimer < 0.5f)
			{
				DrawEntity(entity);
				entity->deathTimer += deltaTime;
			}
			else if (entity->deathTimer < 3)
			{
				entity->deathTimer += deltaTime;
			}
			else
			{
				entity->isDead = false;
				entity->deathTimer = 0;

				if (entity->side == player->side)
				{
					entity->SetSprite(GetPlayerSprite(0));
					entity->RandomizePosition(5, 5,
						0, screenHeight - 12);

					PlaySpawnDidi(1);
				}
				else
				{
					entity->SetSprite(GetPersonSprite(0));
					entity->RandomizePosition(screenWidth / 2, screenWidth - 24,
						0, screenHeight - 12);

					PlayBeep(300, 100.0f);
					PlayBeep(200, 200.0f);
				}
			}
		}
		else
		{
			DrawEntity(entity);
			entity->Update();
		}
	}

	void DrawProjectile(Projectile* projectile)
	{
		SetPixel(projectile->x, projectile->y, 0);

		if (projectile->y > screenHeight || projectile->y < 0 ||
			projectile->x > screenWidth || projectile->x < 0)
		{
			projectile->size = 0;
			projectile->moveX = projectile->moveY = 0;
			projectile->angleX = projectile->angleY = 0;
		}

		if (frameCount % 2 == 0) {
			projectile->Update();
		}
	}

	void EntityFireProjectile(Entity* entity, float angle) {
		projectiles[currentProjectile].angleX = cos(angle);
		projectiles[currentProjectile].angleY = sin(angle);

		projectiles[currentProjectile].side = entity->side;

		projectiles[currentProjectile].size = 1;

		if (entity->side == player->side) {
			projectiles[currentProjectile].x = entity->x + entity->width;
		}
		else {
			projectiles[currentProjectile].x = entity->x - entity->width;
		}

		projectiles[currentProjectile].y = entity->y + (frameCount % 10 >= 5 ? 6 : 5);

		projectiles[currentProjectile].moveX = 0;
		projectiles[currentProjectile].moveY = 0;

		currentProjectile++;

		if (currentProjectile >= PROJECTILE_COUNT) {
			currentProjectile = 0;
		}

		if (entity->side == player->side) PlayBeep(300, 8);
		else PlayBeep(700, 8);
	}

	void DrawCursor() {
		SetPixel(mousePosX, mousePosY - 1, 1);
		SetPixel(mousePosX, mousePosY + 1, 1);

		SetPixel(mousePosX - 1, mousePosY, 1);
		SetPixel(mousePosX + 1, mousePosY, 1);
	}

private:
	void HandleGameplay(float elapsedTime) {
		if (lives <= 0) {
			SwapScreen(Screen::GameOver);

			return;
		}

		starField->DrawField(elapsedTime);

		for (size_t i = 0; i < PROJECTILE_COUNT; i++)
		{
			if (projectiles[i].size > 0) {
				DrawProjectile(&projectiles[i]);
			}
		}

		for (auto& entity : entities)
		{
			UpdateEntity(entity, elapsedTime);

			if (frameCount % 4 == 0) {
				for (size_t i = 0; i < PROJECTILE_COUNT; i++)
				{
					if (projectiles[i].size > 0) {
						if (entity->CheckOverlaps(&projectiles[i])) {
							entity->isDead = true;
							projectiles[i].size = 0;
						}
					}
				}
			}
		}

		if (player->isDead == false) {
			lastShot += elapsedTime;

			if (SHOT_TIMER <= lastShot)
			{
				for (size_t i = 0; i < ENEMY_COUNT; i++)
				{
					if (enemies[i].isDead == true) continue;

					if (enemies[i].WantsToShoot(elapsedTime) == true)
					{
						if (enemies[i].HasToken())
						{
							float angle = GetAngleToTarget(player->collider->GetMiddleX(), player->collider->GetMiddleY(),
								enemies[i].x, enemies[i].y + enemies[i].height / 2);

							EntityFireProjectile(&enemies[i], angle);

							enemies[i].TakeToken();
							SHOOT_TOKENS++;

							lastShot = 0;

							// Only shoot once per frame
							break;
						}
						else if (SHOOT_TOKENS > 0)
						{
							// Do a random check for if we should give this AI a token
							if (rand() % 10 >= 5)  // 50 %
							{
								enemies[i].GiveToken();
								SHOOT_TOKENS--;
							}
						}
					}
				}
			}

			if (GetKey(Key::UP).bHeld || GetKey(Key::W).bHeld) {
				player->moveY -= playerSpeed * elapsedTime;

				if (player->collider->GetTop() < 0) {
					player->moveY = 1;
				}
			}

			if (GetKey(Key::DOWN).bHeld || GetKey(Key::S).bHeld) {
				player->moveY += playerSpeed * elapsedTime;

				if (player->collider->GetBottom() > screenHeight) {
					player->moveY = -1;
				}
			}

			if (GetMouse(0).bPressed) {
				float angle = GetAngleToTarget(mousePosX, mousePosY,
					(player->width), (player->y + 6.0f));

				EntityFireProjectile(player, angle);
			}

			DrawCursor();
		}
		else {
			// FIXME: Reset the token count
		}

		DrawString(screenWidth / 4.0f, 0, std::to_string(lives), 1);
		DrawString(screenWidth / 2.0f, 0, std::to_string(score), 1);
	}

	void SwapScreen(Screen toSwapTo)
	{
		if (toSwapTo == Game) {
			PlaySpawnDidi(1);
			PlaySpawnDidi(2);
			PlaySpawnDidi(3);
		}

		currentScreen = toSwapTo;
	}

public:

	virtual bool OnUserCreate()
	{
		EnableSound();

		currentScreen = Screen::MainMenu;

		starField = new StarSystem(this, 20);

		enemies = new Enemy[ENEMY_COUNT];

		for (size_t i = 0; i < ENEMY_COUNT; i++)
		{
			enemies[i] = Enemy(12, 12, 0, 0, screenHeight);
			enemies[i].RandomizePosition(screenWidth / 2, screenWidth - 24,
				0, screenHeight - 12);
			enemies[i].SetSprite(GetPersonSprite(0), true);
			enemies[i].side = 0;

			entities.push_back(&enemies[i]);
		}

		//explosionSample = LoadAudioSample(L"C:\\Users\\frres\\OneDrive\\Desktop\\Explosion.wav");
		//shootSample = LoadAudioSample(L"C:\\Users\\frres\\OneDrive\\Desktop\\Laser_Shoot.wav");

		player = new Entity(12, 12, 5, 12);
		player->SetSprite(GetPlayerSprite(0), true);
		player->side = 1;

		entities.push_back(player);

		projectiles = new Projectile[PROJECTILE_COUNT];

		return true;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		int centerX = screenWidth / 2.0f;
		int centerY = screenHeight / 2.0f;

		switch (currentScreen)
		{
		case SideScroller::Game:
			HandleGameplay(elapsedTime);
			gameOverTime = 0;
			break;
		case SideScroller::MainMenu:
			starField->DrawField(0);

			DrawString(0, centerY - 24, "SPACE GAME", 2);
			DrawString(0, centerY, "press any key\n to start", 1);

			gameOverTime += elapsedTime;

			if (gameOverTime > 0.5f)
			{
				for (size_t i = 0; i < 256; i++)
				{
					if (GetKey(i).bReleased)
					{
						// Restart game
						score = 0;
						lives = 3;

						// Reset the entities
						for (auto& entity : entities)
						{
							entity->deathTimer = 100;
							UpdateEntity(entity, elapsedTime);
						}

						// Reset the projectiles
						for (size_t i = 0; i < PROJECTILE_COUNT; i++)
						{
							projectiles[i].size = 0;
							projectiles[i].moveX = projectiles[i].moveY = 0;
							projectiles[i].angleX = projectiles[i].angleY = 0;
						}

						// Swap screen to game
						SwapScreen(Screen::Game);
					}
				}
			}
			break;
		case SideScroller::GameOver:
			starField->DrawField(0);

			DrawString(0, centerY - 24, "GAME OVER", 2);
			DrawString(0, centerY, std::to_string(score), 1);

			gameOverTime += elapsedTime;

			if (gameOverTime > 0.5f)
			{
				DrawString(centerX - 8 * 3, centerY + 24, "RETRY?", 1);

				for (size_t i = 0; i < 256; i++)
				{
					if (GetKey(i).bReleased)
					{
						// Restart game
						score = 0;
						lives = 3;

						// Reset the entities
						for (auto& entity : entities)
						{
							entity->deathTimer = 100;
							UpdateEntity(entity, elapsedTime);
						}

						// Reset the projectiles
						for (size_t i = 0; i < PROJECTILE_COUNT; i++)
						{
							projectiles[i].size = 0;
							projectiles[i].moveX = projectiles[i].moveY = 0;
							projectiles[i].angleX = projectiles[i].angleY = 0;
						}

						// Swap screen to game
						SwapScreen(Screen::Game);
					}
				}
			}
			break;
		default:
			break;
		}

		return true;
	}

	// Optional for clean up
	virtual bool OnUserDestroy()
	{
		for (auto& entity : entities) {
			delete entity->sprite;
			delete entity->collider;
		}

		delete[] projectiles;
		delete[] enemies;
		delete player;

		return true;
	}
};