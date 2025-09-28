#include "game.hpp"

#include <algorithm>
#include <memory>

#include "raylib.h"

#include "globals.hpp"
#include "player.hpp"

static constexpr float camera_play = 4;
static constexpr float camera_follow = 0.5f;

Game::Game() : entities{}, gravity(20) {
	using namespace Levels;

	//level = std::make_unique<Level>(Level(lvl1_tileset, lvl1_width, lvl1_height, lvl1_spawn));
	const auto lvl = LoadImage("level.png");
	level = std::make_unique<Level>(Level(lvl, lvl1_spawn));
	UnloadImage(lvl);

	entities.push_back(std::make_unique<Player>(*this, level->get_player_spawn()));

	camera.target = level->get_player_spawn();
	camera.offset = Vector2{
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
	camera.rotation = 0;
	camera.zoom = global::PPU - 1;
}

const Level &Game::get_level() const {
	return *level;
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

	const auto player_pos = ((Player*)&*entities[0])->get_pos();
	const Vector2 d = {
		player_pos.x - camera.target.x,
		player_pos.y - camera.target.y,
	};
	if (std::abs(d.x) > camera_play) {
		const float v = d.x / camera_follow;
		camera.target.x += v * dt;
	}
	if (std::abs(d.y) > camera_play) {
		const float v = d.y / camera_follow;
		camera.target.y += v * dt;
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

	level->draw();

	DrawCircle(0, 0, 0.1, GREEN);
	DrawCircle(0, 1, 0.1, GREEN);

	EndMode2D();

	DrawFPS(10, 10);

	EndDrawing();
}
