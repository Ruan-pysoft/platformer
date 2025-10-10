#pragma once

#include <memory>

#include "gui.hpp"
#include "level.hpp"
#include "player.hpp"
#include "scene.hpp"

class SingleRun : public Scene {
	enum class State {
		Playing,
		Won,
	};

	std::unique_ptr<Level> level = nullptr;
	bool sent_to_main_menu = false;
	Level::Stats total_stats = {};
	Player::Stats total_player_stats = {};
	State state = State::Playing;
	bool initialised_winscreen = false;
	std::vector<Text> text = {};
	std::vector<Button> buttons = {};

public:
	SingleRun();
	~SingleRun();

	void next_level();
	void main_menu();
	void reset_level();

	void update(float dt) override;
	void draw() const override;
	void post_draw() override;
};
