#pragma once

#include "raylib.h"

namespace global {

extern const int FPS;
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

extern const int PPU; // pixels per unit
constexpr inline float units(float pixels) {
	return pixels / PPU;
}
constexpr inline float pixels(float units) {
	return units * PPU;
}
constexpr inline Vector2 worldspace(Vector2 display) {
	return Vector2 { units(display.x), units(WINDOW_HEIGHT - display.y) };
}
constexpr inline Vector2 displayspace(Vector2 world) {
	return Vector2 { pixels(world.x), WINDOW_HEIGHT - pixels(world.y) };
}

}
