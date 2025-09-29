#pragma once

#include <memory>

#include "scene.hpp"

class Game {
	std::unique_ptr<Scene> scene;
	int level_idx;
public:
	Game();

	Scene &get_scene() const;
	void set_scene(std::unique_ptr<Scene> new_scene);
	void next_level();

	void update();
	void draw() const;
	void update_scene();
};
