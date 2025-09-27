#include "game.hpp"

#include "raylib.h"

#include "globals.hpp"
#include "player.hpp"
#include <memory>

Game::Game() : entities{}, gravity(9.81) {
	entities.push_back(std::make_unique<Player>(*this));
	camera.target = Vector2{ 0, 0 };
	camera.offset = Vector2{
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
	camera.rotation = 0;
	camera.zoom = global::PPU;
}

static float ballX = global::WINDOW_WIDTH / 2.0f;
static float ballY = global::WINDOW_HEIGHT / 2.0f;
static const float ballR = 12;
static float ballDx = 80;
static float ballDy = -32;

void Game::update() {
	const float dt = GetFrameTime();

	for (auto &entity : entities) {
		entity->update(dt);
	}

	ballX += ballDx * dt;
	ballY += ballDy * dt;

	if (ballX - ballR < 0) {
		ballX = ballR;
		ballDx = -ballDx;
	} else if (ballX + ballR >= global::WINDOW_WIDTH) {
		ballX = global::WINDOW_WIDTH - ballR - 1;
		ballDx = -ballDx;
	}
	if (ballY - ballR < 0) {
		ballY = ballR;
		ballDy = -ballDy;
	} else if (ballY + ballR >= global::WINDOW_HEIGHT) {
		ballY = global::WINDOW_HEIGHT - ballR - 1;
		ballDy = -ballDy;
	}
}
void Game::draw() {
	BeginDrawing();

	ClearBackground(RAYWHITE);

	DrawCircle(ballX, ballY, ballR, RED);

	BeginMode2D(camera);

	for (const auto &entity : entities) {
		entity->draw();
	}

	DrawCircle(0, 0, 0.1, GREEN);
	DrawCircle(0, 1, 0.1, GREEN);

	EndMode2D();

	DrawFPS(10, 10);

	EndDrawing();
}
