#pragma once

#include "gui.hpp"
#include "scene.hpp"

/*
 * Defines the main menu
 */

class MainMenu : public Scene {
	Button play;
	Button level_select;
	Button quit;
	Text title;
public:
	MainMenu();

	void update(float dt) override;
	void draw() const override;
};
