#include "gui.hpp"

#include "raylib.h"

Button::Button(callback_t on_click, GuiBox box, std::string text)
: Button(on_click, box, text, DEFAULT_TEXT_SIZE)
{ }
Button::Button(callback_t on_click, GuiBox box, std::string text,
	       int text_size)
: Button(on_click, box, text, text_size, DEFAULT_FOCUS_COLOR,
	 DEFAULT_UNFOCUS_COLOR, DEFAULT_TEXT_COLOR)
{ }
Button::Button(callback_t on_click, GuiBox box, std::string text,
	       int text_size, Color color_focus, Color color_unfocus,
	       Color text_color)
: on_click(on_click), box(box), text(text), text_size(text_size),
  color_focus(color_focus), color_unfocus(color_unfocus),
  text_color(text_color), text_width(0), focused(false)
{
	if (GetFontDefault().glyphCount == 0) {
		// NOTE: quick-n-dirty hack: in main(), when the Game object
		// gets created, it creates a MainMenu object, which in turn
		// creates some buttons. But this all happens _before_ the
		// InitWindow is called, which then causes a segfault because
		// the default font does not exist yet
		return;
	}
	update(0);
}

void Button::update(float) {
	/*static int prev_text_size = text_size;

	if (text_size != prev_text_size) {
		prev_text_size = text_size;
		text_width = MeasureText(text.c_str(), text_size);
	}*/
	// TODO: detect somehow if the text has been changed? Otherwise just make it const

	// from raylib/src/rtext.c:1195
	const float spacing = text_size / 10.0f;

	text_width = MeasureTextEx(
		GetFontDefault(), text.c_str(), text_size, spacing
	).x;

	const auto mouse_pos = GetMousePosition();

	focused = CheckCollisionPointRec(mouse_pos, box.rect());
	if (focused && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		on_click();
	}
}
void Button::draw() const {
	DrawRectangleRec(box.rect(), focused ? color_focus : color_unfocus);

	// from raylib/src/rtext.c:1195
	const float spacing = text_size / 10.0f;

	DrawTextEx(
		GetFontDefault(), text.c_str(),
		box.centre_offset({ -text_width/2.0f, -text_size/2.0f, }),
		text_size, spacing, text_color
	);
}

void Text::draw() const {
	// from raylib/src/rtext.c:1195
	const float spacing = font_size / 10.0f;

	DrawTextEx(
		GetFontDefault(), text.c_str(), abs_pos(), font_size, spacing,
		color
	);
}
