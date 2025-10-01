#pragma once

#include <vector>

#include "gui.hpp"
#include "scene.hpp"

class LevelSelect : public Scene {
	Text heading;
	std::vector<Button> level_buttons;
	Button menu;

public:
	LevelSelect();

	void update(float dt) override;
	void draw() const override;
};
