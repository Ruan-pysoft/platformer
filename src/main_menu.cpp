#include "main_menu.hpp"

#include "levels_list.hpp"
#include "raylib.h"

#include "globals.hpp"

void CenteredButton::draw() const {
	if (hovered) {
		DrawRectangleRec(rect(), { 127, 195, 255, 255 });
	} else {
		DrawRectangleRec(rect(), { 63, 127, 255, 255 });
	}

	static const int text_width = MeasureText(text.c_str(), text_size);

	DrawText(
		text.c_str(), centre.x - (text_width / 2.0f),
		centre.y - (text_size / 2.0f), text_size, BLACK
	);
}

MainMenu::MainMenu() {
	play = {
		{ global::WINDOW_WIDTH / 2.0f, global::WINDOW_HEIGHT / 2.0f },
		{ 300, 75 },
		"PLAY", 42, false,
	};
}

void MainMenu::update(float) {
	const auto mouse_pos = GetMousePosition();

	play.centre = {
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};

	play.hovered = CheckCollisionPointRec(mouse_pos, play.rect());
	if (play.hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		transition.next = Levels::make_level(0);
	}
};
void MainMenu::draw() const {
	ClearBackground(RAYWHITE);

	const char *title = "This is a game";
	const int title_size = 50;
	const int title_width = MeasureText(title, title_size);

	DrawText("This is a game", (global::WINDOW_WIDTH - title_width) / 2, 10, title_size, GRAY);

	play.draw();
}
