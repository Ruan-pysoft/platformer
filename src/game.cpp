#include "game.hpp"

#include "globals.hpp"

#include "raylib.h"

Game::Game() : player(*this), gravity(9.81) {
}

static float ballX = global::WINDOW_WIDTH / 2.0f;
static float ballY = global::WINDOW_HEIGHT / 2.0f;
static const float ballR = 12;
static float ballDx = 50;
static float ballDy = -20;

void Game::update() {
	const float dt = GetFrameTime();

	player.update(dt);

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

	player.draw();

	DrawFPS(10, 10);

	EndDrawing();
}
