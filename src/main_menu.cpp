#include "main_menu.hpp"

#include <memory>

#include "raylib.h"

#include "globals.hpp"
#include "level_scene.hpp"
#include "level_select.hpp"

MainMenu::MainMenu()
: play {
	[this]() {
		transition.next = LevelScene::from_level_nr(0);
	},
	GuiBox::floating_x({ 0, 100 }, { 400, 75 }), "PLAY"
  }, level_select {
	[this]() {
		transition.next = std::make_unique<LevelSelect>();
	},
	GuiBox::floating_x({ 0, 200 }, { 400, 75 }), "LEVEL SELECT"
  }, quit {
	[]() {
		global::quit = true;
	},
	GuiBox::floating_x({ 0, 300 }, { 400, 75 }), "QUIT"
  },
  title { "This is a game", 50, { 0, 10 }, true, GRAY }
{ }

void MainMenu::update(float dt) {
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
