#pragma once

#include "scene.hpp"

class MainMenu : public Scene {
public:
	MainMenu();

	void update(float) override;
	void draw() const override;
};
