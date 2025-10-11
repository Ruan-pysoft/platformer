#pragma once

#include <memory>

#include "scene.hpp"

/*
 * Manages the scene interface for playing a single level
 */

class Level;
class LevelScene : public Scene {
	std::unique_ptr<Level> level;
	bool sent_to_main_menu = false;

public:
	LevelScene(std::unique_ptr<Level> level);
	~LevelScene();
	static std::unique_ptr<LevelScene> from_level_nr(int level_nr);

	void next_level();
	void prev_level();
	void main_menu();
	void reset_level();

	void update(float dt) override;
	void draw() const override;
	void post_draw() override;
};
