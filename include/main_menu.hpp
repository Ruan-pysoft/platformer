#pragma once

#include "scene.hpp"

class MainMenu : public Scene {
	SceneAction action = SceneAction::Continue;
public:
	MainMenu();

	void update(float) override;
	void draw() const override;
	SceneAction get_action() const override;
};
