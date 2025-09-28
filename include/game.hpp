#pragma once

#include <memory>
#include <vector>

#include "raylib.h"

#include "entity.hpp"
#include "level.hpp"

class Game {
	std::unique_ptr<Level> level;
	std::vector<std::unique_ptr<Entity>> entities;
	Camera2D camera;
public:
	float gravity;

	Game();

	const Level &get_level() const;

	void update();
	void draw();
};
