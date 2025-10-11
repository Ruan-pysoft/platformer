#include "game.hpp"

#include <iostream>
#include <memory>

#include "raylib.h"

#include "globals.hpp"
#include "main_menu.hpp"
#include "scene.hpp"

// historical fps view, only compiled into debug builds
#ifdef DEBUG
#define FPS_BUFFER_SIZE (1 << 8)
#define FPS_BUFFER_PERIOD 5.0f
#define FRAME_WIDTH (FPS_BUFFER_PERIOD / FPS_BUFFER_SIZE)

float fps_buffer[FPS_BUFFER_SIZE] = {};
int fps_buffer_start = 0;
const int fps_draw_height = 100;
#endif /* DEBUG */

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

	// the previous scene's std::unique_ptr goes out of scope at the end of
	// this function, which then calls the std::unique_ptr's destructor,
	// which calls the scene's destructor and then deallocates the memory.
	// In this way resource cleanup and memory deallocation is handled
	// without any programmer intervention or runtime garbage collection
	scene.swap(new_scene);
}

void Game::update() {
	const float dt = GetFrameTime();

	// update the historical fps view (only in debug builds)
#ifdef DEBUG
	static float acc_dt = 0;
	static int acc_frames = 0;

	acc_dt += GetFrameTime();
	++acc_frames;

	if (acc_dt >= FRAME_WIDTH) {
		const int frames = acc_dt / FRAME_WIDTH;
		const float fps = acc_frames / acc_dt;
		for (int i = 0; i < frames; ++i) {
			const int fps_buffer_idx = (fps_buffer_start + i)&(FPS_BUFFER_SIZE-1);
			fps_buffer[fps_buffer_idx] = fps;
		}
		fps_buffer_start += frames;
		acc_dt = 0;
		acc_frames = 0;
	}
#endif /* DEBUG */

	scene->update(dt);
}
void Game::draw() const {
	BeginDrawing();

#ifdef DEBUG
	const double pre_draw_time = GetTime();
#endif

	scene->draw();

	const int fps_height = 20;
	const int fps_maxwidth = MeasureText("1000 FPS", fps_height);
	const int fps_margin = 10;

	DrawFPS(
		global::WINDOW_WIDTH - fps_maxwidth - fps_margin,
		global::WINDOW_HEIGHT - fps_height - fps_margin
	);

	// display both the historical fps view as well as the time spent
	// drawing the frame (only debug builds)
#ifdef DEBUG
	WaitTime(42. / 1000 / 1000);
	const double post_draw_time = GetTime();

	const double draw_time = post_draw_time - pre_draw_time; // number of seconds it took to draw the frame

	const int millis = draw_time * 1000;
	const int sub_millis = int(draw_time * 1000 * 10) % 10;
	//const int micros = draw_time * 1000 * 1000;

	const int frametime_height = fps_height;
	const int frametime_margin = fps_margin;

	const std::string frametime_text = std::to_string(millis) + '.' + std::to_string(sub_millis) + " ms";
	//const std::string frametime_text = std::to_string(micros) + " µs";
	const int frametime_width = MeasureText(frametime_text.c_str(), frametime_height);

	DrawText(
		frametime_text.c_str(),
		global::WINDOW_WIDTH - frametime_width - frametime_margin,
		global::WINDOW_HEIGHT - fps_height - fps_margin - frametime_height - frametime_margin,
		frametime_height, GREEN
	);
#endif

#ifdef DEBUG
	float max_fps = 0.01f;
	for (int i = 0; i < FPS_BUFFER_SIZE; ++i) {
		max_fps = std::max(max_fps, fps_buffer[i]);
	}

	for (int i = 0; i < FPS_BUFFER_SIZE; ++i) {
		const int fps_buffer_idx = (fps_buffer_start + i)&(FPS_BUFFER_SIZE-1);
		const int x = i + 10;
		const int y = global::WINDOW_HEIGHT - fps_draw_height - 10;
		const int height = fps_buffer[fps_buffer_idx] * fps_draw_height / max_fps;

		if (height) DrawLine(
			x, y + fps_draw_height - height,
			x, y + fps_draw_height - 1,
			{ 0, 255, 0, 127 }
		);
	}
#endif /* DEBUG */

	EndDrawing();
}
void Game::update_scene() {
	// check if scenes should be switched
	scene->post_draw();
	if (scene->transition.next) set_scene(std::move(scene->transition.next));
}
