#include "main_menu.hpp"

#include <memory>

#include "raylib.h"

#include "globals.hpp"
#include "level_select.hpp"
#include "levels_list.hpp"

MainMenu::MainMenu()
: play {
	[this]() {
		transition.next = Levels::make_level(0);
	},
	GuiBox::floating_x({ 0, 250 }, { 400, 75 }), "PLAY"
  }, level_select {
	[this]() {
		transition.next = std::make_unique<LevelSelect>();
	},
	GuiBox::floating_x({ 0, 350 }, { 400, 75 }), "LEVEL SELECT"
  }, quit {
	[]() {
		global::quit = true;
	},
	GuiBox::floating_x({ 0, 450 }, { 400, 75 }), "QUIT"
  },
  title { "This is a game", 50, { 0, 10 }, true, GRAY }
{ }

void MainMenu::update(float dt) {
	title.pos.x = global::WINDOW_WIDTH / 2.0f;

	play.update(dt);
	level_select.update(dt);
	quit.update(dt);
};
void MainMenu::draw() const {
	ClearBackground(RAYWHITE);

	title.draw();

	play.draw();
	level_select.draw();
	quit.draw();
}
