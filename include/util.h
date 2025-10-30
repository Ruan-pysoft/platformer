#pragma once

#include <stdbool.h>

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * collision detection & resolution helper
 */
struct Collision {
	Vector2 dist; // how the first object needs to be moved to no longer overlap
	Vector2 new_pos; // where the first object needs to be moved to to no longer overlap
	bool x_touches; // does 0 mean x is touching, or x is not colliding at all?
	bool y_touches; // does 0 mean y is touching, or y is not colliding at all?
};
struct Collision collide(Rectangle from, Rectangle to);

#ifdef __cplusplus
}
#endif
