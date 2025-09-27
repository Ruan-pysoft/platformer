#pragma once

#include <memory>
#include <vector>

#include "raylib.h"

#include "entity.hpp"

class Game {
	std::vector<std::unique_ptr<Entity>> entities;
	Camera2D camera;
public:
	float gravity;

	Game();

	void update();
	void draw();
};
