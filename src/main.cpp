#include "raylib.h"

#include "actions.hpp"
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

	InitWindow(
		global::WINDOW_WIDTH, global::WINDOW_HEIGHT,
		"Hello Window"
	);

	InitAudioDevice();

	SetTargetFPS(global::FPS);

	while (!WindowShouldClose()) {
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

const float SCALE = 1.0f;
const int FPS = 60;
const int WINDOW_WIDTH = 800 * SCALE;
const int WINDOW_HEIGHT = 600 * SCALE;

const int PPU = 20 * SCALE;

}
