#pragma once

#include <functional>
#include <string>

#include "raylib.h"

#include "globals.hpp"
struct GuiBox {
	Vector2 pos;
	Vector2 size;
	bool x_rel_centre;
	bool y_rel_centre;

	static GuiBox absolute(Vector2 pos, Vector2 size) {
		return { pos, size, false, false, };
	}
	static GuiBox floating_x(Vector2 pos, Vector2 size) {
		return { pos, size, true, false, };
	}
	static GuiBox floating_y(Vector2 pos, Vector2 size) {
		return { pos, size, false, true, };
	}
	static GuiBox floating(Vector2 pos, Vector2 size) {
		return { pos, size, true, true, };
	}

	Vector2 abs_pos() const {
		if (!x_rel_centre && !y_rel_centre) return pos;

		const Vector2 window_centre = {
			global::WINDOW_WIDTH / 2.0f,
			global::WINDOW_HEIGHT / 2.0f,
		};
		const Vector2 adjust = {
			x_rel_centre ? window_centre.x - size.x/2 : 0,
			y_rel_centre ? window_centre.y - size.y/2 : 0,
		};
		return {
			adjust.x + pos.x,
			adjust.y + pos.y,
		};
	}
	Vector2 centre_offset(Vector2 by) const {
		const auto topleft = abs_pos();
		return {
			topleft.x + size.x/2 + by.x,
			topleft.y + size.y/2 + by.y,
		};
	}
	Rectangle rect() const {
		const auto topleft = abs_pos();
		return { topleft.x, topleft.y, size.x, size.y, };
	}
};

struct Button {
	static constexpr int CENTERED = -1;
	static constexpr int DEFAULT_TEXT_SIZE = 42;
	static constexpr Color DEFAULT_FOCUS_COLOR = { 127, 195, 255, 255 };
	static constexpr Color DEFAULT_UNFOCUS_COLOR = { 63, 127, 255, 255 };
	static constexpr Color DEFAULT_TEXT_COLOR = BLACK;
	using callback_t = std::function<void()>;

	callback_t on_click;
	GuiBox box;
	std::string text;
	int text_size;
	Color color_focus;
	Color color_unfocus;
	Color text_color;

	float text_width;
	bool focused;

	Button(callback_t on_click, GuiBox box, std::string text);
	Button(callback_t on_click, GuiBox box, std::string text,
	       int text_size);
	Button(callback_t on_click, GuiBox box, std::string text,
	       int text_size, Color color_focus, Color color_unfocus,
	       Color text_color);

	void update(float);
	void draw() const;
};

struct Text {
	std::string text;
	int font_size;
	Vector2 pos;
	bool centered;
	Color color;

	Vector2 abs_pos() const {
		// from raylib/src/rtext.c:1195
		const float spacing = font_size / 10.0f;

		const auto size = MeasureTextEx(
			GetFontDefault(), text.c_str(), font_size, spacing
		);

		if (centered) {
			const float x_centre = global::WINDOW_WIDTH / 2.0f;
			return { x_centre - size.x/2, pos.y };
		} else return pos;
	}

	void draw() const;
};
