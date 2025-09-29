#include "game.hpp"

#include <iostream>
#include <memory>

#include "raylib.h"

#include "globals.hpp"
#include "main_menu.hpp"
#include "scene.hpp"

Game::Game() {
	set_scene(std::make_unique<MainMenu>());
}

Scene &Game::get_scene() const {
	return *scene;
}
void Game::set_scene(std::unique_ptr<Scene> new_scene) {
	if (new_scene == nullptr) {
		std::cerr << "WARN: tried to load a nullptr level!" << std::endl;
		std::cerr << "INFO: loading main menu instead" << std::endl;

		scene = std::make_unique<MainMenu>();
		return;
	}

	scene.swap(new_scene);
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
	if (scene->transition.next) set_scene(std::move(scene->transition.next));
}
