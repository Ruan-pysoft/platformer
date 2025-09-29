#pragma once

#include "raylib.h"

class Level;
class Player {
	Vector2 pos;
	Vector2 vel;
	static constexpr Vector2 size = Vector2 { 1.0f, 2.0f };

	Player();

	bool on_ground(Level &level);

	void resolve_collisions_x(Level &level);
	void resolve_collisions_y(Level &level);
public:
	static Player &get_player();

	Vector2 get_pos() const;
	void reset(Vector2 pos);

	void update(Level &level, float dt);
	void draw() const;
};
