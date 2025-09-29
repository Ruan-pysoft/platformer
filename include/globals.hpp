#pragma once

#include "raylib.h"

namespace global {

extern const int FPS;
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const int PPU; // pixels per unit
const float STAGE_WIDTH = WINDOW_WIDTH / (float)PPU;
const float STAGE_HEIGHT = WINDOW_HEIGHT / (float)PPU;

constexpr inline float units(float pixels) {
	return pixels / PPU;
}
constexpr inline float pixels(float units) {
	return units * PPU;
}

}
