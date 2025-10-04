#include "level_select.hpp"

#include <memory>
#include <string>

#include "raylib.h"

#include "level_scene.hpp"
#include "levels_list.hpp"
#include "main_menu.hpp"
#include "singlerun.hpp"

LevelSelect::LevelSelect()
: heading { "Levels", 50, { 0, 10 }, true, GRAY },
  level_buttons {},
  menu {
	[this]() {
		transition.next = std::make_unique<MainMenu>();
	},
	GuiBox::floating_x({ -175, 75 }, { 300, 75 }), "BACK"
  }, single_run {
	[this]() {
		transition.next = std::make_unique<SingleRun>();
	},
	GuiBox::floating_x({ 175, 75 }, { 300, 75 }), "CHALLENGE"
  }
{
	const float level_btn_size = 50;
	const float level_btn_space = 25;
	const int level_btn_cols = 5;
	const float level_btn_first_row_y = 175;
	const float level_btn_total_width =
		(level_btn_size + level_btn_space)
		* (level_btn_cols - 1);

	int row = 0;
	int col = 0;

	for (int i = 0; i < int(Levels::levels.size()); ++i) {
		const int lvl_idx = i;
		const std::string lvl_str = std::to_string(i+1);

		const float y =
			level_btn_first_row_y
			+ (level_btn_size + level_btn_space) * row;
		const float x_offset =
			(level_btn_size + level_btn_space) * col
			- level_btn_total_width/2;

		level_buttons.push_back({
			[this, lvl_idx]() {
				transition.next = LevelScene::from_level_nr(lvl_idx);
			},
			GuiBox::floating_x(
				{ x_offset, y },
				{ level_btn_size, level_btn_size }
			), lvl_str
		});

		++col;
		if (col >= level_btn_cols) {
			col = 0;
			++row;
		}
	}
}

void LevelSelect::update(float dt) {
	for (auto &level_btn : level_buttons) {
		level_btn.update(dt);
	}
	menu.update(dt);
	single_run.update(dt);
}
void LevelSelect::draw() const {
	ClearBackground(RAYWHITE);

	heading.draw();

	for (const auto &level_btn : level_buttons) {
		level_btn.draw();
	}
	menu.draw();
	single_run.draw();
}
