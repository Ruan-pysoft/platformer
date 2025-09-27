#pragma once

#include "raylib.h"

class Game;

class Player {
	Game &game;
	Vector2 pos;
	Vector2 vel;
	static constexpr Vector2 size = Vector2 { 1, 2 };

	bool on_ground();

public:
	Player(Game &game);

	void update(float dt);
	void draw();
};
