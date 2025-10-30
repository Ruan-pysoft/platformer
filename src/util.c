#include "util.h"

#include "raylib.h"
#include "raymath.h"

static inline Vector2 Rectangle_pos(Rectangle this) {
	return (Vector2) { this.x, this.y };
}
static inline Vector2 Rectangle_span(Rectangle this) {
	return (Vector2) { this.width, this.height };
}
static inline Vector2 Rectangle_centre(Rectangle this) {
	return (Vector2) {
		this.x + this.width/2,
		this.y + this.height/2,
	};
}

struct Collision collide(Rectangle from, Rectangle to) {
	struct Collision res = {0};

	res.new_pos = Rectangle_pos(from);

	const Vector2 from_centre = Rectangle_centre(from);
	const Vector2 to_centre = Rectangle_centre(to);

	// can only collide in the x-direction if it overlaps in the y-direction
	const bool might_collide_horisontal =
		(from.y <= to.y && from.y + from.height >= to.y)
		|| (from.y <= to.y + to.height && from.y + from.height >= to.y + to.height)
		|| (from.y <= to.y + to.height && from.y + from.height <= to.y)
	;
	// can only collide in the y-direction if it overlaps in the x-direction
	const bool might_collide_vertical =
		(from.x <= to.x && from.x + from.width >= to.x)
		|| (from.x <= to.x + to.width && from.x + from.width >= to.x + to.width)
		|| (from.x <= to.x + to.width && from.x + from.width <= to.x)
	;

	if (might_collide_horisontal) {
		if (from_centre.x < to_centre.x) {
			const float from_near_edge = from.x + from.width;
			const float to_near_edge = to.x;

			const float near_dist = to_near_edge - from_near_edge;

			if (near_dist <= 0) {
				res.x_touches = true;
				res.dist.x = near_dist;

				res.new_pos.x = to.x - from.width;
			}
		} else {
			const float from_near_edge = from.x;
			const float to_near_edge = to.x + to.width;

			const float near_dist = to_near_edge - from_near_edge;

			if (near_dist >= 0) {
				res.x_touches = true;
				res.dist.x = near_dist;

				res.new_pos.x = to.x + to.width;
			}
		}
	}

	if (might_collide_vertical) {
		if (from_centre.y < to_centre.y) {
			const float from_near_edge = from.y + from.height;
			const float to_near_edge = to.y;

			const float near_dist = to_near_edge - from_near_edge;

			if (near_dist <= 0) {
				res.y_touches = true;
				res.dist.y = near_dist;

				res.new_pos.y = to.y - from.height;
			}
		} else {
			const float from_near_edge = from.y;
			const float to_near_edge = to.y + to.height;

			const float near_dist = to_near_edge - from_near_edge;

			if (near_dist >= 0) {
				res.y_touches = true;
				res.dist.y = near_dist;

				res.new_pos.y = to.y + to.height;
			}
		}
	}

	return res;
}
