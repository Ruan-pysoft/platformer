#include "overlay.hpp"

#include "globals.hpp"

Overlay::Overlay() : text{}, buttons{} { }

Overlay &Overlay::add_text(Text text) {
	this->text.push_back(text);

	return *this;
}
Overlay &Overlay::add_button(Button button) {
	buttons.push_back(button);

	return *this;
}
Text *Overlay::get_text(size_t ix) {
	if (ix > text.size()) return nullptr;
	return &text[ix];
}
Button *Overlay::get_button(size_t ix) {
	if (ix > buttons.size()) return nullptr;
	return &buttons[ix];
}

void Overlay::update(float dt) {
	for (auto &e : buttons) {
		e.update(dt);
	}
}
void Overlay::draw() const {
	DrawRectangle(
		0, 0, global::WINDOW_WIDTH, global::WINDOW_HEIGHT,
		{ 195, 195, 255, 127 }
	);

	for (auto &e : text) {
		e.draw();
	}
	for (auto &e : buttons) {
		e.draw();
	}
}
