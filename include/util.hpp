#pragma once

#include "raylib.h"

namespace util {

struct Collision {
	Vector2 dist = { 0, 0 }; // how the first object needs to be moved to no longer overlap
	Vector2 new_pos = { 0, 0 }; // where the first object needs to be moved to to no longer ovrerlap
	bool x_touches = 0; // does 0 mean x is touching, or x is not colliding at all?
	bool y_touches = 0; // does 0 mean y is touching, or y is not colliding at all?
};

Collision collide(Rectangle from, Rectangle to);

}
