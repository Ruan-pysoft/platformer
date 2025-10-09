#include "level_scene.hpp"

#include <memory>

#include "level.hpp"
#include "levels_list.hpp"
#include "main_menu.hpp"

LevelScene::LevelScene(std::unique_ptr<Level> level) : level(std::move(level))
{ }
LevelScene::~LevelScene() = default;
std::unique_ptr<LevelScene> LevelScene::from_level_nr(int level_nr) {
	return std::make_unique<LevelScene>(Levels::make_level(level_nr));
}

void LevelScene::next_level() {
	if (level == nullptr) {
		main_menu();
		return;
	}

	const int curr = level->get_level_nr();

	level = Levels::make_level(curr + 1);
	if (level == nullptr) main_menu();
}
void LevelScene::prev_level() {
	if (level == nullptr) {
		main_menu();
		return;
	}

	const int curr = level->get_level_nr();

	if (curr == 0) {
		main_menu();
		return;
	}
	level = Levels::make_level(curr - 1);
}
void LevelScene::main_menu() {
	if (sent_to_main_menu) return;
	transition.next = std::make_unique<MainMenu>();
	sent_to_main_menu = true;
}
void LevelScene::reset_level() {
	if (level == nullptr) {
		main_menu();
		return;
	}

	const int curr = level->get_level_nr();

	level = Levels::make_level(curr);
}

void LevelScene::update(float dt) {
	if (level != nullptr) level->update(dt);
}
void LevelScene::draw() const {
	if (level != nullptr) level->draw();
}
void LevelScene::post_draw() {
	if (transition.next != nullptr) return;
	if (level == nullptr) {
		main_menu();
		return;
	}
	switch (level->change) {
		case Level::Change::None: break;
		case Level::Change::Prev: {
			prev_level();
		} break;
		case Level::Change::Next: {
			next_level();
		} break;
		case Level::Change::Reset: {
			reset_level();
		} break;
		case Level::Change::MainMenu: {
			main_menu();
		} break;
	}
}
