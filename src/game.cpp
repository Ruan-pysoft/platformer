#include "game.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>

#include "raylib.h"

#include "globals.hpp"
#include "level.hpp"
#include "main_menu.hpp"
#include "player.hpp"
#include "scene.hpp"

static const struct {
	const char *file;
	Vector2 spawn;
} levels[] = {
	{ "levels/level1.png", Levels::lvl1_spawn },
	{ "levels/level2.png", Levels::lvl2_spawn },
};

Game::Game() {
	load_main_menu();
}

Scene &Game::get_scene() const {
	return *scene;
}
void Game::set_scene(std::unique_ptr<Scene> new_scene) {
	scene.swap(new_scene);
}
void Game::load_level(int lvl) {
	using namespace Levels;
	if (lvl < 0 || level_idx >= int(sizeof(levels)/sizeof(*levels))) load_main_menu();

	const auto level_img = LoadImage(levels[lvl].file);
	set_scene(std::make_unique<Level>(Level(level_img, levels[lvl].spawn)));
	UnloadImage(level_img);

	level_idx = lvl;
}
void Game::next_level() {
	using namespace Levels;

	++level_idx;
	if (level_idx < 0 || level_idx >= int(sizeof(levels)/sizeof(*levels))) {
		load_main_menu();
		return;
	}

	load_level(level_idx);
}
void Game::load_main_menu() {
	level_idx = -1;

	set_scene(std::make_unique<MainMenu>());
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
			load_main_menu();
		} break;
	}
}
