#pragma once

#include "player.hpp"

class Game {
	Player player;
public:
	float gravity;

	Game();

	void update();
	void draw();
};
