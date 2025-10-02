#pragma once

struct Config;
namespace global {

extern Config config;
extern const int FPS;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern const int PPU; // pixels per unit
const float STAGE_WIDTH = WINDOW_WIDTH / (float)PPU;
const float STAGE_HEIGHT = WINDOW_HEIGHT / (float)PPU;
extern bool quit;
extern const int PHYSICS_FPS;

constexpr inline float units(float pixels) {
	return pixels / PPU;
}
constexpr inline float pixels(float units) {
	return units * PPU;
}

}
