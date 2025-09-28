#pragma once

#include "raylib.h"

#include "entity.hpp"

class Game;

class Player : public Entity {
	Game &game;
	Vector2 pos;
	Vector2 vel;
	static constexpr Vector2 size = Vector2 { 1.0f, 2.0f };

	bool on_ground();

	void resolve_collisions_x();
	void resolve_collisions_y();
public:
	Player(Game &game, Vector2 spawn);

	Vector2 get_pos() const;

	void update(float dt);
	void draw() const;
};
