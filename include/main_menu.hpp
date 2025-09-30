#pragma once

#include <string>

#include "raylib.h"

#include "scene.hpp"

struct CenteredButton {
	Vector2 centre;
	Vector2 size;
	std::string text;
	int text_size;
	bool hovered;

	Rectangle rect() const {
		return {
			centre.x - size.x / 2,
			centre.y - size.y / 2,
			size.x, size.y,
		};
	}

	void draw() const;
};

class MainMenu : public Scene {
	CenteredButton play;
public:
	MainMenu();

	void update(float) override;
	void draw() const override;
};
