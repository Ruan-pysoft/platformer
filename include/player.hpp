#pragma once

#include "raylib.h"

#include "entity.hpp"

class Game;

class Player : public Entity {
	Game &game;
	Vector2 pos;
	Vector2 vel;
	static constexpr Vector2 size = Vector2 { 1, 2 };

	bool on_ground();

public:
	Player(Game &game);

	void update(float dt);
	void draw() const;
};
