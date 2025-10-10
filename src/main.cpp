#include "raylib.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "actions.hpp"
#include "config.hpp"
#include "input_manager.hpp"
#include "game.hpp"
#include "globals.hpp"

int main() {
	Game game;
	const InputManager &inp_mgr = InputManager::get();

	for (auto &map : Action::INIT_KEYMAP_ONCE) {
		map.action.register_key(map.key, map.on_press);
	}
	for (auto &map : Action::INIT_KEYMAP_STARTSTOP) {
		map.action.register_key(map.key);
	}
	for (auto &map : Action::INIT_KEYMAP_SUSTAIN) {
		map.action.register_key(map.key);
	}

	if (!std::filesystem::exists("data/")) {
		if (!std::filesystem::create_directory("data/")) {
			std::cerr << "Failed creating game data folder!" << std::endl;
			return 1;
		}
	}

	if (std::filesystem::exists("data/game.cfg")) {
		std::ifstream cfg_file("data/game.cfg");
		global::config = Config::read(cfg_file);
		cfg_file.close();
	} else {
		global::config.had_uninits = true;
	}

	if (global::config.had_uninits) {
		std::ofstream cfg_file("game.cfg");
		global::config.write(cfg_file);
		cfg_file.close();
	}

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	global::WINDOW_WIDTH = global::config.window_width;
	global::WINDOW_HEIGHT = global::config.window_height;

	InitWindow(
		global::WINDOW_WIDTH, global::WINDOW_HEIGHT,
		"Hello Window"
	);
	switch (global::config.window_state) {
		case WindowState::Windowed: break;
		case WindowState::Borderless: {
			ToggleBorderlessWindowed();
		} break;
		case WindowState::Fullscreen: {
			ToggleFullscreen();
#ifdef __linux__
			std::cerr << "WARNING: Fullscreen seems to be buggy on linux" << std::endl;
#endif
		} break;
	}
	SetExitKey(0); // unset escape to exit

	InitAudioDevice();

	SetTargetFPS(global::FPS);

	while (!WindowShouldClose() && !global::quit) {
		if (IsWindowFullscreen()) {
			const int monitor = GetCurrentMonitor();
			global::WINDOW_WIDTH = GetMonitorWidth(monitor);
			global::WINDOW_HEIGHT = GetMonitorHeight(monitor);
		} else {
			global::WINDOW_WIDTH = GetScreenWidth();
			global::WINDOW_HEIGHT = GetScreenHeight();
		}
		inp_mgr.handleInputs();
		game.update();
		game.draw();
		game.update_scene();
	}

	CloseAudioDevice();

	CloseWindow();

	return 0;
}

namespace global {

Config config;
const float SCALE = 1.0f;
const int FPS = 60;
int WINDOW_WIDTH = 800 * SCALE;
int WINDOW_HEIGHT = 600 * SCALE;
bool quit = false;
const int PHYSICS_FPS = 32;

const int PPU = 20 * SCALE;

}
