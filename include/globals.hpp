#pragma once

struct Config;
namespace global {

extern Config config;
extern const int FPS;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern const int PPU; // pixels per unit
extern bool quit;
extern const int PHYSICS_FPS;
extern const char *DATA_DIR;
extern const char *PERSONAL_BESTS_FILE;

}
