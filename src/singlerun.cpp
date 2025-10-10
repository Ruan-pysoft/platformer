#include "singlerun.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "globals.hpp"
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
}
SingleRun::~SingleRun() = default;

void SingleRun::next_level() {
	if (level == nullptr) {
		main_menu();
		return;
	}

	const int curr = level->get_level_nr();
	total_stats += level->get_stats();

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
	total_stats += level->get_stats();

	level = Levels::make_level(curr, true);
}

void SingleRun::update(float dt) {
	if (state == State::Playing) {
		if (level != nullptr) level->update(dt);
	} else {
		if (!initialised_winscreen) {
			initialised_winscreen = true;

			PBFile pbs_file = PBFile::load();
			const std::string key = "challenge_run";
			auto pb = pbs_file.get(key);

			const bool new_pb = pb == nullptr || total_stats.better_than(*pb);

			Text &stats_text = text[1];
			Text &pb_text = text[2];

			const std::string stats_label = "Time / Jumps / Deaths & Resets: ";
			std::string stats_value;

			const unsigned seconds = total_stats.time / global::PHYSICS_FPS;
			const unsigned frames = total_stats.time % global::PHYSICS_FPS;
			stats_value += std::to_string(seconds);
			stats_value += ";";
			if (frames < 10) stats_value += "0";
			stats_value += std::to_string(frames);

			stats_value += " / ";

			stats_value += std::to_string(total_stats.total_jumps());

			stats_value += " / ";

			stats_value += std::to_string(total_stats.total_respawns());

			stats_text.text = stats_label + stats_value;
			if (new_pb) {
				stats_text.text += " (New PB!)";
			}

			const std::string pb_label = "Personal Best: ";
			std::string pb_value;

			if (pb == nullptr) {
				pb_value = "N/A";
			} else {
				const unsigned seconds = pb->time / global::PHYSICS_FPS;
				const unsigned frames = pb->time % global::PHYSICS_FPS;
				pb_value += std::to_string(seconds);
				pb_value += ";";
				if (frames < 10) pb_value += "0";
				pb_value += std::to_string(frames);

				pb_value += " / ";

				pb_value += std::to_string(pb->total_jumps());

				pb_value += " / ";

				pb_value += std::to_string(pb->total_respawns());
			}

			pb_text.text = pb_label + pb_value;

			if (new_pb) {
				pbs_file.set(key, total_stats);
				pbs_file.save();
			}
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
		case Level::Change::None: break;
		case Level::Change::Prev: {
			std::cerr << "Something went very wrong!" << std::endl;
			main_menu();
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
