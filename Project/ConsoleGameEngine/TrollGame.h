#pragma once

bool DEBUG_CURSOR = false;

class Projectile
{
private:
	float moveX = 0, moveY = 0;

	const float BASE_SPEED = 30.0f;

public:
	void ResetMove()
	{
		moveX = moveY = 0;
	}

	void ResetProjectile()
	{
		ResetMove();
		side = 0;
		x = y = 0;
		velocityX = velocityY = 0;
		size = 0;
		lastHit = 1;
	}

	int side = 0;

	int x, y = 0;
	int size = 0;
	float velocityX, velocityY = 0;

	Collider* collider;

	float lastHit = 1;

	void Update()
	{
		moveX += velocityX * (BASE_SPEED * Time->elapsedTime);
		moveY += velocityY * (BASE_SPEED * Time->elapsedTime);

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
};

class Entity
{
protected:
	olcConsoleGameEngine* renderer = nullptr;

public:
	void SetRenderer(olcConsoleGameEngine* renderer)
	{
		this->renderer = renderer;
	}

	bool enabled = true;

	Collider* collider = nullptr;

	int side = 0;

	Sprite* sprite;
	int width, height;

	bool isDead = false;
	float deathTimer = 0;

	int x, y;

	float moveX = 0, moveY = 0;

	void BaseUpdate()
	{
		y += moveY;
		x += moveX;

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

		return collider->CheckCollision(projectile->collider);
	}

	void UpdateSprite(int spriteIndex = 0)
	{
		sprite->SetSpriteIndex(spriteIndex);
		sprite->SetupSprite();
	}

public:
	Entity(int width, int height, int x, int y, SPRITES::SpriteTag spriteTag) {
		this->x = x;
		this->y = y;

		moveX = moveY = 0;

		this->width = width;
		this->height = height;

		sprite = new Sprite(width, height, spriteTag);

		collider = new Collider(sprite->GetCurrentSprite(), width, height);
	};

	Entity() = default;
};

class TrollGame : public olcConsoleGameEngine
{
private:
	enum Screen {
		Game,
		Intro,
		MainMenu,
		GameOver
	};

	const static int EXPLOSION_INTEX = 3;

	Screen currentScreen;

	UIBox* box;

	Sprite* projectileSprite;
	Sprite* bridgeStrutSprite;
	Sprite* explosionSprite;

	Collider* mouseCollider;

	Entity* people;
	const static int PEOPLE_COUNT = 5;

	Entity* player;
	const float playerSpeed = 55.0f;

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

	int targetX, targetY;

	int playerY = 0;

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

	void TakeLife()
	{
		lives--;

		if (lives <= 0) {
			PlayBeep(500, 50);
			PlayBeep(400, 100);
			PlayBeep(300, 500);
		}
		else {
			PlayBeep(400, 50);
			PlayBeep(300, 100);
		}
	}

	void AddScore(int amount)
	{
		// Do some crazy and wild shit
		// Like add score
		score += amount;

		if (amount <= 10)
		{
			if (amount < 0)
			{
				PlayBeep(500, 50);
				PlayBeep(300, 100);
			}
			else
			{
				PlayBeep(700, 50);
				PlayBeep(900, 10);
			}
		}
		else
		{
			PlayBeep(300, 50.0f);
			PlayBeep(600, 50.0f);
			PlayBeep(900, 100.0f);
		}
	}

	void UpdateEntity(Entity* entity, float deltaTime)
	{
		if (entity->isDead == true) {
			entity->enabled = false;
			entity->isDead = false;
		}
		else
		{
			DrawSprite(entity->x, entity->y, entity->sprite);
			entity->Update();
		}
	}

	void DrawProjectile(Projectile* projectile)
	{
		for (size_t x = 0; x < projectileSprite->GetWidth(); x++)
		{
			for (size_t y = 0; y < projectileSprite->GetHeight(); y++)
			{
				if (projectileSprite->GetGlyph(x, y) != '.')
				{
					// Draw something here
					int spr = GetPixelFromChar(projectileSprite->GetGlyph(x, y));

					SetPixel(projectile->x + x, projectile->y + y, spr);
				}
			}
		}

		if (projectile->y > screenHeight - 20)
		{
			AddScore(-10);
			projectile->size = 0;
		}
		else {
			if (projectile->y < 0 || projectile->x > screenWidth || projectile->x < 0)
			{
				AddScore(5);
				projectile->ResetProjectile();
			}
		}
		projectile->Update();
	}

	void FireProjectile(int startX, int startY, int side, float angle) {
		// Calculate angle
		projectiles[currentProjectile].velocityX = cos(angle);
		projectiles[currentProjectile].velocityY = sin(angle);

		// Set damaging side
		projectiles[currentProjectile].side = side;

		// Set size and position
		projectiles[currentProjectile].size = 1;
		projectiles[currentProjectile].x = startX;
		projectiles[currentProjectile].y = startY;

		// Reset movement
		projectiles[currentProjectile].ResetMove();

		projectiles[currentProjectile].lastHit = 1;

		// Loop through projectiles pool
		currentProjectile++;
		if (currentProjectile >= PROJECTILE_COUNT) currentProjectile = 0;

		// Projectile sound
		PlayBeep(700, 8);
	}

	int cursorFrame = 0;

	void DrawCursor()
	{
		if (frameCount % 50 == 0) {
			cursorFrame++;
			cursorFrame %= 9;
		}

		int cursorCurrent = cursorFrame;

		cursorCurrent %= 3;

		SetPixel(mousePosX - 1, mousePosY - 1, cursorCurrent);
		SetPixel(mousePosX - 1, mousePosY, cursorCurrent);

		cursorCurrent++;
		cursorCurrent %= 3;

		SetPixel(mousePosX - 1, mousePosY + 1, cursorCurrent);
		SetPixel(mousePosX, mousePosY - 1, cursorCurrent);

		cursorCurrent++;
		cursorCurrent %= 3;

		SetPixel(mousePosX + 1, mousePosY + 1, cursorCurrent);
		SetPixel(mousePosX, mousePosY + 1, cursorCurrent);

		cursorCurrent++;
		cursorCurrent %= 3;

		SetPixel(mousePosX + 1, mousePosY, cursorCurrent);
		SetPixel(mousePosX + 1, mousePosY - 1, cursorCurrent);
	}

	void DrawBridge()
	{
		DrawLine(0, screenHeight - 20, screenWidth, screenHeight - 20, PIXEL_SOLID, FG_WHITE);
		//DrawLine(0, screenHeight - 19, screenWidth, screenHeight - 19, PIXEL_SOLID, FG_WHITE);
		DrawLine(0, screenHeight - 18, screenWidth, screenHeight - 18, PIXEL_SOLID, FG_WHITE);

		DrawLine(20, screenHeight - 19, 0, screenHeight, PIXEL_SOLID, FG_WHITE);
		DrawLine(21, screenHeight - 19, 1, screenHeight, PIXEL_SOLID, FG_WHITE);

		DrawLine(screenWidth - 20, screenHeight - 19, screenWidth, screenHeight, PIXEL_SOLID, FG_WHITE);
		DrawLine(screenWidth - 21, screenHeight - 19, screenWidth - 1, screenHeight, PIXEL_SOLID, FG_WHITE);
	}

public:

	virtual bool OnUserCreate()
	{
		EnableSound();

		box = new UIBox(this, 0, 0, screenWidth, 12);

		mouseCollider = new Collider(6, 6);
		mouseCollider->offX = -3;
		mouseCollider->offY = -3;

		bridgeStrutSprite = new Sprite(12, 12, SPRITES::BridgeStrut);

		currentScreen = Screen::MainMenu;

		people = new Entity[PEOPLE_COUNT];

		for (size_t i = 0; i < PEOPLE_COUNT; i++)
		{
			people[i] = Entity(12, 12, 0, 0, SPRITES::Person);
			people[i].RandomizePosition(screenWidth / 2, screenWidth - 24,
				0, screenHeight - 12);
			people[i].side = 2;
			people[i].isDead = true;
			people[i].SetRenderer(this);
			people[i].enabled = false;

			entities.push_back(&people[i]);
		}

		player = new Entity(12, 12, 5, screenHeight - 12, SPRITES::Troll);
		player->side = 1;
		player->isDead = false;

		player->SetRenderer(this);

		entities.push_back(player);

		projectileSprite = new Sprite(12, 12, SPRITES::Projectile);
		projectiles = new Projectile[PROJECTILE_COUNT];

		Collider* tempC = new Collider(projectileSprite->GetSprite(), projectileSprite->GetWidth(), projectileSprite->GetHeight());

		for (size_t i = 0; i < PROJECTILE_COUNT; i++)
		{
			projectiles[i].collider = new Collider(tempC->width, tempC->height);
			projectiles[i].collider->offX = tempC->offX;
			projectiles[i].collider->offY = tempC->offY;
		}

		// Cleanup our fucking mess
		delete tempC;

		targetY = screenHeight;
		targetX = screenWidth / 2;

		return true;
	}

	virtual bool OnUserUpdate()
	{
		mouseCollider->UpdatePosition(mousePosX, mousePosY);

		int centerX = screenWidth / 2.0f;
		int centerY = screenHeight / 2.0f;

		switch (currentScreen)
		{
		case TrollGame::Game:
			HandleGameplay();
			gameOverTime = 0;
			break;
		case TrollGame::MainMenu:

			DrawString(0, centerY - 24, "LET\'S BE A");
			DrawString(centerX, centerY - 24, "TROLL", 2);
			DrawString(0, centerY + 15, "press any key\n to start", 1);

			gameOverTime += Time->elapsedTime;

			if (gameOverTime > 0.5f)
			{
				for (size_t i = 0; i < 256; i++)
				{
					if (GetKey(i).bReleased)
					{
						// Swap screen to game
						SwapScreen(Screen::Intro);
					}
				}
			}

			if (frameCount % 1000 == 0)
			{
				PlayBeep(800.0f, 200.0f);
				PlayBeep(600.0f, 200.0f);
				PlayBeep(700.0f, 200.0f);
			}

			if (frameCount % 1000 == 200)
			{
				PlayBeep(700.0f, 200.0f);
				PlayBeep(500.0f, 200.0f);
				PlayBeep(600.0f, 200.0f);
			}

			if (frameCount % 1000 == 400)
			{
				PlayBeep(600.0f, 200.0f);
				PlayBeep(400.0f, 200.0f);
				PlayBeep(500.0f, 200.0f);
			}

			if (frameCount % 1000 == 600)
			{
				PlayBeep(600.0f, 200.0f);
				PlayBeep(800.0f, 200.0f);
				PlayBeep(700.0f, 200.0f);
			}

			if (frameCount % 1000 == 800)
			{
				PlayBeep(700.0f, 200.0f);
				PlayBeep(900.0f, 200.0f);
				PlayBeep(600.0f, 500.0f);
			}

			break;
		case TrollGame::GameOver:
			DrawString(0, centerY - 24, "GAME OVER", 2);
			DrawString(0, centerY, std::to_string(score), 1);

			gameOverTime += Time->elapsedTime;

			if (gameOverTime > 0.5f)
			{
				DrawString(centerX - 8 * 3, centerY + 24, "RETRY?", 1);

				for (size_t i = 0; i < 256; i++)
				{
					if (GetKey(i).bReleased)
					{
						// Swap screen to game
						SwapScreen(Screen::Game);
					}
				}
			}
			break;
		case TrollGame::Intro:
			DrawString(0, 0, "Protect humans and\nbridge from harm\nwith your magic\n (mouse)\nCollect tolls\nfrom humans\ntrying to pass\n (spacebar)");

			gameOverTime += Time->elapsedTime;

			DrawSprite(5, screenHeight - 12, player->sprite);

			if (gameOverTime > 0.5f)
			{
				DrawString(centerX - 8 * 3, centerY + 24, "Ready?", 1);

				for (size_t i = 0; i < 256; i++)
				{
					if (GetKey(i).bReleased)
					{
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
		//delete[] enemies;
		delete player;

		return true;
	}

private:
	void ResetGameplay()
	{
		// Restart game
		score = 0;
		lives = 3;

		// Reset the entities
		for (auto& entity : entities)
		{
			if (entity->side != player->side) {
				entity->deathTimer = 0;
				entity->enabled = false;

				UpdateEntity(entity, Time->elapsedTime);
			}
		}

		// Reset the projectiles
		for (size_t i = 0; i < PROJECTILE_COUNT; i++)
		{
			projectiles[i].ResetProjectile();
		}
	}

	void HandleGameplay() {
		if (lives <= 0) {
			SwapScreen(Screen::GameOver);

			return;
		}

		DrawBridge();

		if (frameCount %= 10) {
			// Reset the targetX every 10th frame
			targetX = screenWidth * 0.75f;
		}

		// Two mouse position updates for best collision detection
		mouseCollider->UpdatePosition(mousePosX, mousePosY);

		bool drawingProjectiles = false;

		for (auto& entity : entities)
		{
			if (entity->enabled == false) continue;

			UpdateEntity(entity, Time->elapsedTime);

			if (entity->side != player->side) {
				// THIS IS NOT THE PLAYER
				if (entity->collider->CheckCollision(player->collider) && !entity->isDead) {
					entity->isDead = true;

					AddScore(100);
					continue;
				}

				for (size_t i = 0; i < PROJECTILE_COUNT; i++)
				{
					if (projectiles[i].size > 0)
					{
						DrawProjectile(&projectiles[i]);

						if (mouseCollider->CheckCollision(projectiles[i].collider))
						{
							projectiles[i].velocityX = (projectiles[i].x > mousePosX) ? 3 : -3;
							projectiles[i].velocityY = -3;

							if (projectiles[i].lastHit > 0.1f)
							{
								PlayBeep(800, 50.0f);
								projectiles[i].lastHit = 0;
							}
						}

						projectiles[i].lastHit += Time->elapsedTime;

						if (entity->CheckOverlaps(&projectiles[i])) {
							entity->isDead = true;
							projectiles[i].size = 0;

							AddScore(-10);
						}
					}
				}

				drawingProjectiles = true;
			}
		}

		if (drawingProjectiles == false)
		{
			for (size_t i = 0; i < PROJECTILE_COUNT; i++)
			{
				if (projectiles[i].size > 0)
				{
					DrawProjectile(&projectiles[i]);

					if (mouseCollider->CheckCollision(projectiles[i].collider))
					{
						projectiles[i].velocityX = (projectiles[i].x > mousePosX) ? 3 : -3;
						projectiles[i].velocityY = -3;

						if (projectiles[i].lastHit > 0.1f)
						{
							PlayBeep(800, 50.0f);
							projectiles[i].lastHit = 0;
						}
					}
				}
			}
		}

		for (size_t i = 0; i < PEOPLE_COUNT; i++)
		{
			if (people[i].enabled)
			{
				people[i].moveX = 0;

				if (frameCount % 10 == 0)
				{
					people[i].moveX = -2;
				}

				if (people[i].x <= 0)
				{
					people[i].enabled = false;
					TakeLife();
				}
				else if (people[i].x < targetX)
				{
					targetX = people[i].x;
				}
			}
		}

		if (GetKey(Key::SPACE).bPressed)
		{
			playerY = 40;
		}

		if (GetKey(Key::SPACE).bHeld)
		{
			player->y = screenHeight - (playerY);

			if (frameCount % 10 == 0)
			{
				playerY--;

				if (playerY <= 12) playerY = 12;
			}
		}
		else
		{
			player->y = screenHeight - 12;
		}

		// Randomly send a civilian
		if (rand() % 500 == frameCount % 500)
		{
			int i = rand() % PEOPLE_COUNT;

			if (people[i].enabled == false)
			{
				people[i].x = screenWidth;
				people[i].y = screenHeight - 32;

				people[i].moveX = people[i].moveY = 0;

				people[i].enabled = true;
			}
		}

		// Fire projectiles randomly
		if (rand() % 300 == frameCount % 300)
		{
			int posX = (rand() % 100 > 49) ? 5 : screenWidth - 5;;
			int posY = 5;

			FireProjectile(posX, posY, 0, GetAngleToTarget(targetX, targetY, posX, posY));
		}

		if (rand() % 700 == frameCount % 700)
		{
			int posX = rand() % screenWidth;
			int posY = 5;

			FireProjectile(posX, posY, 0, GetAngleToTarget(targetX, targetY, posX, posY));
		}

		if (DEBUG_CURSOR)
		{
			// Debug cursor collider
			for (size_t x = 0; x < mouseCollider->width; x++)
			{
				for (size_t y = 0; y < mouseCollider->height; y++)
				{
					Draw(mousePosX + mouseCollider->offX + x,
						mousePosY + mouseCollider->offY + y, PIXEL_SOLID, FG_RED);
				}
			}
		}
		else
		{
			DrawCursor();
		}

		box->Draw();
		DrawString(screenWidth - 20, 2, std::to_string(lives), 1);
		DrawString(5, 2, std::to_string(score), 1);
	}

	void SwapScreen(Screen toSwapTo)
	{
		gameOverTime = 0;

		if (toSwapTo == Game) {
			ResetGameplay();

			PlaySpawnDidi(1);
			PlaySpawnDidi(2);
			PlaySpawnDidi(3);
		}

		currentScreen = toSwapTo;
	}
};