#include "raylib.h"

#include "game.hpp"
#include "globals.hpp"

int main() {
	Game game;

	InitWindow(
		global::WINDOW_WIDTH, global::WINDOW_HEIGHT,
		"Hello Window"
	);

	InitAudioDevice();

	SetTargetFPS(global::FPS);

	while (!WindowShouldClose()) {
		game.update();
		game.draw();
	}

	CloseAudioDevice();

	CloseWindow();

	return 0;
}

namespace global {

const int FPS = 60;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int PPU = 16;

}
