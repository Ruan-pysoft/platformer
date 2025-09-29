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

void Game::update() {
	const float dt = GetFrameTime();

	scene->update(dt);
}
void Game::draw() const {
	BeginDrawing();

	scene->draw();

	const int fps_height = 20;
	const int fps_maxwidth = MeasureText("1000 FPS", fps_height);
	const int fps_margin = 10;

	DrawFPS(
		global::WINDOW_WIDTH - fps_maxwidth - fps_margin,
		global::WINDOW_HEIGHT - fps_height - fps_margin
	);

	EndDrawing();
}
void Game::update_scene() {
	if (scene->transition.next) set_scene(std::move(scene->transition.next));
}
