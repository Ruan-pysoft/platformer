#include "game.hpp"

#include <cstdlib>
#include <memory>

#include "raylib.h"

#include "globals.hpp"
#include "level.hpp"
#include "player.hpp"
#include "scene.hpp"

static const struct {
	const char *file;
	Vector2 spawn;
} levels[] = {
	{ "levels/level1.png", Levels::lvl1_spawn },
};

Game::Game() {
	level_idx = -1;

	next_level();
}

Scene &Game::get_scene() const {
	return *scene;
}
void Game::set_scene(std::unique_ptr<Scene> new_scene) {
	scene.swap(new_scene);
}
void Game::next_level() {
	using namespace Levels;

	++level_idx;
	if (level_idx < 0 || level_idx >= sizeof(levels)/sizeof(*levels)) exit(1);

	const auto level_img = LoadImage(levels[level_idx].file);
	set_scene(std::make_unique<Level>(Level(level_img, levels[level_idx].spawn)));
	UnloadImage(level_img);
}

static float ballX = global::WINDOW_WIDTH / 2.0f;
static float ballY = global::WINDOW_HEIGHT / 2.0f;
static const float ballR = 12;
static float ballDx = 80;
static float ballDy = -32;

void Game::update() {
	const float dt = GetFrameTime();

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

	scene->update(dt);
}
void Game::draw() const {
	BeginDrawing();

	scene->draw();

	DrawCircle(ballX, ballY, ballR, RED);

	DrawFPS(10, 10);

	EndDrawing();
}
void Game::update_scene() {
	switch (scene->get_action()) {
		case SceneAction::Continue: break;
		case SceneAction::NextLevel: {
			next_level();
		} break;
		case SceneAction::MainMenu: {
			exit(1);
		} break;
	}
}
