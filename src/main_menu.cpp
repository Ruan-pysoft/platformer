#include "main_menu.hpp"

#include "raylib.h"

#include "globals.hpp"
#include "levels_list.hpp"

MainMenu::MainMenu()
: play {
	[this]() {
		transition.next = Levels::make_level(0);
	},
	GuiBox::floating_x({ 0, 250 }, { 300, 75 }), "PLAY"
  }, quit {
	[]() {
		global::quit = true;
	},
	GuiBox::floating_x({ 0, 350 }, { 300, 75 }), "QUIT"
  },
  title { "This is a game", 50, { 0, 10 }, true, GRAY }
{ }

void MainMenu::update(float dt) {
	title.pos.x = global::WINDOW_WIDTH / 2.0f;

	play.update(dt);
	quit.update(dt);
};
void MainMenu::draw() const {
	ClearBackground(RAYWHITE);

	title.draw();

	play.draw();
	quit.draw();
}
