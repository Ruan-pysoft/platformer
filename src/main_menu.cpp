#include "main_menu.hpp"

#include "raylib.h"

#include "globals.hpp"

static const Vector2 play_button_centre = {
	global::WINDOW_WIDTH / 2.0f,
	global::WINDOW_HEIGHT / 2.0f,
};
static const Vector2 play_button_size = { 300, 75 };
static const Rectangle play_button_rect = {
	play_button_centre.x - play_button_size.x / 2,
	play_button_centre.y - play_button_size.y / 2,
	play_button_size.x, play_button_size.y,
};
static int play_button_rounding = 16;
static const char *play_button_text = "PLAY";
static const int play_button_text_size = 42;
static bool play_button_hovered = false;

MainMenu::MainMenu() {
	play_button_hovered = false;
}

void MainMenu::update(float) {
	const auto mouse_pos = GetMousePosition();

	play_button_hovered = CheckCollisionPointRec(mouse_pos, play_button_rect);
	if (play_button_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		action = SceneAction::NextLevel;
	}
};
void MainMenu::draw() const {
	ClearBackground(RAYWHITE);

	const char *title = "This is a game";
	const int title_size = 50;
	const int title_width = MeasureText(title, title_size);

	DrawText("This is a game", (global::WINDOW_WIDTH - title_width) / 2, 10, title_size, GRAY);

	if (play_button_hovered) {
		DrawRectangleRec(play_button_rect, { 195, 211, 255, 255 } );
		DrawRectangleRounded(
			play_button_rect, play_button_rounding, 64,
			{ 127, 195, 255, 255 }
		);
	} else {
		DrawRectangleRec(play_button_rect, { 127, 127, 195, 255 } );
		DrawRectangleRounded(
			play_button_rect, play_button_rounding, 64,
			{ 63, 127, 255, 255 }
		);
	}

	static const int play_button_text_width = MeasureText(play_button_text, play_button_text_size);

	DrawText(
		play_button_text, play_button_centre.x - (play_button_text_width / 2.0f),
		play_button_centre.y - (play_button_text_size / 2.0f),
		play_button_text_size,
		BLACK
	);
}
SceneAction MainMenu::get_action() const {
	return action;
}
