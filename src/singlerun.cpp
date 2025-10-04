#include "singlerun.hpp"

#include <iostream>
#include <memory>

#include "level.hpp"
#include "levels_list.hpp"
#include "main_menu.hpp"

SingleRun::SingleRun()
{
	level = Levels::make_level(0, true);

	buttons.push_back({
		[this]() {
			main_menu();
		},
		GuiBox::floating_x({ 0, 175 }, { 500, 75 }), "MAIN MENU"
	});
	text.push_back({ "Challenge completed!", 50, { 0, 12.5 }, true, BLACK });
	text.push_back({ "", 24, { 0, 75 }, true, BLACK });
	text.push_back({ "", 24, { 0, 100 }, true, BLACK });
	text.push_back({ "", 24, { 0, 125 }, true, BLACK });
}
SingleRun::~SingleRun() = default;

void SingleRun::next_level() {
	if (level == nullptr) {
		main_menu();
		return;
	}

	const int curr = level->get_level_nr();
	total_stats.accumulate(level->get_stats());
	total_player_stats.accumulate(level->get_player_stats());
	--total_player_stats.times_spawned;

	level = Levels::make_level(curr + 1, true);
	if (level == nullptr) {
		state = State::Won;
	}
}
void SingleRun::main_menu() {
	if (sent_to_main_menu) return;
	transition.next = std::make_unique<MainMenu>();
	sent_to_main_menu = true;
}
void SingleRun::reset_level() {
	if (level == nullptr) {
		main_menu();
		return;
	}

	const int curr = level->get_level_nr();
	total_stats.accumulate(level->get_stats());
	total_player_stats.accumulate(level->get_player_stats());

	level = Levels::make_level(curr, true);
}

void SingleRun::update(float dt) {
	if (state == State::Playing) {
		if (level != nullptr) level->update(dt);
	} else {
		Text &time_text = text[1];
		if (time_text.text.size() == 0) {
			time_text.text = "Completion time: ";
			const int seconds = total_stats.level_ticks / global::PHYSICS_FPS;
			const int frames = total_stats.level_ticks % global::PHYSICS_FPS;
			time_text.text += std::to_string(seconds);
			time_text.text += ";";
			if (frames < 10) time_text.text += "0";
			time_text.text += std::to_string(frames);
		}
		Text &jumps_text = text[2];
		if (jumps_text.text.size() == 0) {
			jumps_text.text = "Total jumps: ";
			jumps_text.text += std::to_string(
				total_player_stats.jumps + total_player_stats.double_jumps
			);
		}
		Text &deaths_text = text[3];
		if (deaths_text.text.size() == 0) {
			deaths_text.text = "Total deaths/resets: ";
			deaths_text.text += std::to_string(
				total_player_stats.times_spawned
			);
		}

		for (auto &e : buttons) e.update(dt);
	}
}
void SingleRun::draw() const {
	if (state == State::Playing) {
		if (level != nullptr) level->draw();
	} else {
		ClearBackground(RAYWHITE);

		for (const auto &e : buttons) e.draw();
		for (const auto &e : text) e.draw();
	}
}
void SingleRun::post_draw() {
	if (transition.next != nullptr) return;

	if (state == State::Won) return;

	if (level == nullptr) {
		main_menu();
		return;
	}
	switch (level->change) {
		case LevelChange::None: break;
		case LevelChange::Prev: {
			std::cerr << "Something went very wrong!" << std::endl;
			main_menu();
		} break;
		case LevelChange::Next: {
			next_level();
		} break;
		case LevelChange::Reset: {
			reset_level();
		} break;
		case LevelChange::MainMenu: {
			main_menu();
		} break;
	}
}
