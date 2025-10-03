#include "util.hpp"

namespace util {

Collision collide(Rectangle from, Rectangle to) {
	Collision res = {};

	res.new_pos = { from.x, from.y };

	const Vector2 from_centre = {
		from.x + from.width/2,
		from.y + from.height/2,
	};
	const Vector2 to_centre = {
		to.x + to.width/2,
		to.y + to.height/2,
	};

	const bool might_collide_horisontal =
		(from.y <= to.y && from.y + from.height >= to.y)
		|| (from.y <= to.y + to.height && from.y + from.height >= to.y + to.height)
		|| (from.y <= to.y + to.height && from.y + from.height <= to.y);
	;
	const bool might_collide_vertical =
		(from.x <= to.x && from.x + from.width >= to.x)
		|| (from.x <= to.x + to.width && from.x + from.width >= to.x + to.width)
		|| (from.x <= to.x + to.width && from.x + from.width <= to.x);
	;

	if (might_collide_horisontal) {
		if (from_centre.x < to_centre.x) {
			const float from_near_edge = from.x + from.width;
			/* const float from_far_edge = from.x; */
			const float to_near_edge = to.x;
			/* const float to_far_edge = to.x + to.width; */

			// smallest (signed) x-distance from `from` to `to`
			const float near_dist = to_near_edge - from_near_edge;
			// greatest (signed) x-distance from `from` to `to`
			/* const float far_dist = to_far_edge - from_far_edge; */

			// right edge of `from` is touching or past left edge of `to`
			if (near_dist <= 0) {
				res.x_touches = true;
				res.dist.x = near_dist;

				res.new_pos.x = to.x - from.width;
			}
		} else {
			const float from_near_edge = from.x;
			const float to_near_edge = to.x + to.width;

			// smallest (signed) x-distance from `from` to `to`
			const float near_dist = to_near_edge - from_near_edge;

			// left edge of `from` is touching or past right edge of `to`
			if (near_dist >= 0) {
				res.x_touches = true;
				res.dist.x = near_dist;

				res.new_pos.x = to.x + to.width;
			}
		}
	}

	// y is implemented as a copy of x
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

}
