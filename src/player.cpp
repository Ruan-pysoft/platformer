#include "player.hpp"

#include <algorithm>
/*#include <iostream>*/

#include "actions.hpp"
#include "level.hpp"

static const float jump_vel = 12;
static const float walk_acc = 16;
static const float walk_dec = 32;
static const float walk_speed = 20;
static const float friction = 12;
static bool try_jump = false;
static bool try_djump = false;
static bool walk_left = false;
static bool walk_right = false;
static bool walking = false;
static bool has_djumped = false;
static bool was_killed = false;
static bool achieved_goal = false;
static bool flying = false;

static constexpr float EPS = 1.0f / 1024;

Player::Player() : pos { 0, 0 }, vel { 0, 0 } {
	Action::Jump.register_cb([](float) {
		try_jump = true;
	});
	Action::DoubleJump.register_cb([]() {
		try_djump = true;
	});
	Action::Left.register_cb([](float) {
		walk_left = true;
	});
	Action::Right.register_cb([](float) {
		walk_right = true;
	});
	Action::Fly.register_cb([](float) {
		flying = true;
	});
}

bool Player::on_ground(Level &level) {
	if (pos.y >= 0) return true;

	for (int dx = -1; dx <= 1; ++dx) {
		const Vector2 check_point = {
			pos.x + dx,
			pos.y + 0.5f,
		};
		const auto collider = level.get_collider(check_point.x, check_point.y);

		if (collider.width == 0 && collider.height == 0) continue;
		if (collider.x >= pos.x + size.x/2 || collider.x + collider.width <= pos.x - size.x/2) continue;

		const TileType type = level.get_tile_type(check_point.x, check_point.y);

		if (type != TileType::Solid) continue;

		if (collider.y == pos.y) return true;

		const float bottom_dist_near = collider.y - pos.y;
		const float bottom_dist_far = (collider.y + collider.height) - pos.y;

		const bool inside_bottom = bottom_dist_near <= 0 && bottom_dist_far >= 0;

		if (inside_bottom) return true;
	}

	return false;
}

void Player::resolve_collisions_x(Level &level) {
	const float left_edge = pos.x - size.x/2;
	const float right_edge = pos.x + size.x/2;

	for (int dy = -2; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			const Vector2 check_point = {
				pos.x + dx,
				pos.y - 0.5f + dy,
			};
			const auto collider = level.get_collider(check_point.x, check_point.y);
			const auto type = level.get_tile_type(check_point.x, check_point.y);

			if (type == TileType::Empty) continue;
			if (collider.width == 0 && collider.height == 0) continue;
			if (collider.y + EPS >= pos.y || collider.y + collider.height - EPS <= pos.y - size.y) continue;

			const float left_dist_near = left_edge - (collider.x + collider.width);
			const float left_dist_far  = left_edge - collider.x;
			const float right_dist_near = collider.x - right_edge;
			const float right_dist_far  = (collider.x + collider.width) - right_edge;

			const bool touching_left = left_dist_near < 0 && left_dist_far > 0;
			const bool touching_right = right_dist_near < 0 && right_dist_far > 0;
			const bool inside_left = left_dist_near < -EPS && left_dist_far > 0;
			const bool inside_right = right_dist_near < -EPS && right_dist_far > 0;

			const float left_overlap = -left_dist_near; // only if inside_left is true
			const float right_overlap = -right_dist_near; // only if inside_right is true

			if (touching_left || touching_right) switch (type) {
				case TileType::Solid: if (inside_left || inside_right) {
					/*std::cerr << "Overlapping! (" << pos.x << ',' << pos.y << ')' << std::endl;
					std::cerr << "At relative block " << dx << ',' << dy << std::endl;
					std::cerr << "Left distances:  " << left_dist_near << " // " << left_dist_far << std::endl;
					std::cerr << "Right distances: " << right_dist_near << " // " << right_dist_far << std::endl;
					std::cerr << "Left overlap:  " << left_overlap << std::endl;
					std::cerr << "Right overlap: " << right_overlap << std::endl;*/

					if (left_overlap < right_overlap) {
						/*std::cerr << "Resolving collision on the left rightwards..." << std::endl;*/
						pos.x = collider.x + collider.width + size.x/2;
						if (vel.x < 0) vel.x = 0;
					} else {
						/*std::cerr << "Resolving collision on the right leftwards..." << std::endl;*/
						pos.x = collider.x - size.x/2;
						if (vel.x > 0) vel.x = 0;
					}
				} break;
				case TileType::Danger: {
					was_killed = true;
				} break;
				case TileType::Goal: {
					achieved_goal = true;
				} break;
				case TileType::Empty: break;
			}
		}
	}
}
void Player::resolve_collisions_y(Level &level) {
	if (pos.y > 0) pos.y = 0;

	const float top_edge = pos.y - size.y;
	const float bottom_edge = pos.y;

	for (int dy = -2; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			const Vector2 check_point = {
				pos.x + dx,
				pos.y - 0.5f + dy,
			};
			const auto collider = level.get_collider(check_point.x, check_point.y);
			const auto type = level.get_tile_type(check_point.x, check_point.y);

			if (collider.width == 0 && collider.height == 0) continue;
			if (collider.x + EPS >= pos.x + size.x/2 || collider.x + collider.width - EPS <= pos.x - size.x/2) continue;

			const float top_dist_near = top_edge - (collider.y + collider.height);
			const float top_dist_far = top_edge - collider.y;
			const float bottom_dist_near = collider.y - bottom_edge;
			const float bottom_dist_far = (collider.y + collider.height) - bottom_edge;

			const bool touching_top = top_dist_near < 0 && top_dist_far > 0;
			const bool touching_bottom = bottom_dist_near < 0 && bottom_dist_far > 0;
			const bool inside_top = top_dist_near < -EPS && top_dist_far > 0;
			const bool inside_bottom = bottom_dist_near < -EPS && bottom_dist_far > 0;

			const float top_overlap = -top_dist_near; // only if inside_top is true
			const float bottom_overlap = -bottom_dist_near; // only if inside_bottom is true

			if (touching_top || touching_bottom) switch (type) {
				case TileType::Solid: if (inside_top || inside_bottom) {
					/*std::cerr << "Overlapping! (" << pos.x << ',' << pos.y << ')' << std::endl;
					std::cerr << "At relative block " << dx << ',' << dy << std::endl;
					std::cerr << "Top distances:    " << top_dist_near << " // " << top_dist_far << std::endl;
					std::cerr << "Bottom distances: " << bottom_dist_near << " // " << bottom_dist_far << std::endl;
					std::cerr << "Top overlap:    " << top_overlap << std::endl;
					std::cerr << "Bottom overlap: " << bottom_overlap << std::endl;*/
					if (top_overlap < bottom_overlap) {
						/*std::cerr << "Resolving collision on the top downwards..." << std::endl;*/
						pos.y = collider.y + collider.height + size.y;
						if (vel.y < 0) vel.y = 0;
					} else {
						/*std::cerr << "Resolving collision on the bottom upwards..." << std::endl;*/
						pos.y = collider.y;
						if (vel.y > 0) vel.y = 0;
					}
				} break;
				case TileType::Danger: {
					was_killed = true;
				} break;
				case TileType::Goal: {
					achieved_goal = true;
				} break;
				case TileType::Empty: break;
			}
		}
	}
}

Player &Player::get_player() {
	static Player instance{};

	return instance;
}

Vector2 Player::get_pos() const {
	return pos;
}
void Player::reset(Vector2 pos) {
	this->pos = pos;

	try_jump = false;
	try_djump = false;
	walk_left = false;
	walk_right = false;
	walking = false;
	has_djumped = false;
	was_killed = false;
	achieved_goal = false;
	flying = false;
}

void Player::update(Level &level, float dt) {
	const bool grounded = on_ground(level);

	if (was_killed || achieved_goal) return;

	/*if (try_jump) std::cerr << "Trying to jump!" << std::endl;
	if (try_djump) std::cerr << "Trying to double jump!" << std::endl;
	if (walk_left) std::cerr << "Walking left!" << std::endl;
	if (walk_right) std::cerr << "Walking right!" << std::endl;*/

	if (try_jump && grounded) {
		vel.y = -jump_vel;
	}
	if (try_djump && !grounded && !has_djumped) {
		vel.y = -jump_vel;
		has_djumped = true;
	}
	if (walk_left) {
		walking = true;
		if (vel.x > 0) {
			vel.x -= walk_dec * dt;
		} else if (vel.x > -walk_speed) {
			vel.x -= walk_acc * dt;
			if (vel.x < -walk_speed) vel.x = -walk_speed;
		}
	}
	if (walk_right) {
		walking = true;
		if (vel.x < 0) {
			vel.x += walk_dec * dt;
		} else if (vel.x < walk_speed) {
			vel.x += walk_acc * dt;
			if (vel.x > walk_speed) vel.x = walk_speed;
		}
	}
	if (flying) {
		vel.y = std::min(vel.y, -jump_vel / 2.0f);
	}

	if (on_ground(level)) {
		has_djumped = false;

		vel.y = std::min(0.f, vel.y);

		if (!walking) {
			if (friction * dt >= std::abs(vel.x)) vel.x = 0;
			else if (vel.x > 0) vel.x -= friction * dt;
			else vel.x += friction * dt;
		}
	} else {
		if (!flying) vel.y += level.gravity * dt;
	}

	if (std::abs(vel.y) <= std::abs(vel.x)) {
		pos.x += vel.x * dt;
		resolve_collisions_x(level);

		pos.y += vel.y * dt;
		resolve_collisions_y(level);
	} else {
		pos.y += vel.y * dt;
		resolve_collisions_y(level);

		pos.x += vel.x * dt;
		resolve_collisions_x(level);
	}

	walking = false;
	try_jump = false;
	try_djump = false;
	walk_left = false;
	walk_right = false;
	flying = false;

	if (was_killed) level.reset();
	if (achieved_goal) level.complete();
}
void Player::draw() const {
	DrawRectangleV(
		Vector2{ pos.x - size.x/2, pos.y - size.y },
		size,
		BLACK
	);
}
