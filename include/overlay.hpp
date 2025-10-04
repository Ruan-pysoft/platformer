#pragma once

#include <vector>

#include "gui.hpp"

class Overlay {
	std::vector<Text> text;
	std::vector<Button> buttons;
public:
	Overlay();

	Overlay &add_text(Text text);
	Overlay &add_button(Button button);

	Text *get_text(size_t ix);
	Button *get_button(size_t ix);

	void update(float dt);
	void draw() const;
};
