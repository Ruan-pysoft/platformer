#include <iostream>

namespace ray {
#include "raylib.h"
}

int main() {
	ray::InitWindow(800, 600, "Hello Window");

	ray::InitAudioDevice();

	ray::SetTargetFPS(60);

	while (!ray::WindowShouldClose()) {
		ray::BeginDrawing();

		ray::ClearBackground(ray::RAYWHITE);

		ray::DrawFPS(10, 10);

		ray::EndDrawing();
	}

	ray::CloseAudioDevice();

	ray::CloseWindow();

	return 0;
}
