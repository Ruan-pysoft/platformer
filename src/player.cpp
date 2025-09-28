#include "player.hpp"

#include <algorithm>
/*#include <iostream>*/

#include "actions.hpp"
#include "game.hpp"

static const float jump_vel = 12;
static const float walk_acc = 16;
static const float walk_dec = 32;
static const float walk_speed = 20;
static const float friction = 12;
static bool walking = false;
static bool has_djumped = false;

static constexpr float EPS = 1.0f / 1024 / 1024;

Player::Player(Game &game, Vector2 spawn)
	: game(game),
	  pos(spawn),
	  vel { 0, 0 } {
	Action::Jump.register_cb([this](float) {
		if (on_ground()) {
			vel.y = -jump_vel;
		}
	});
	Action::DoubleJump.register_cb([this]() {
		if (!on_ground() && !has_djumped) {
			vel.y = -jump_vel;
			has_djumped = true;
		}
	});
	Action::Left.register_cb([this](float dt) {
		walking = true;
		if (vel.x > 0) {
			vel.x -= walk_dec * dt;
		} else if (vel.x > -walk_speed) {
			vel.x -= walk_acc * dt;
			if (vel.x < -walk_speed) vel.x = -walk_speed;
		}
	});
	Action::Right.register_cb([this](float dt) {
		walking = true;
		if (vel.x < 0) {
			vel.x += walk_dec * dt;
		} else if (vel.x < walk_speed) {
			vel.x += walk_acc * dt;
			if (vel.x > walk_speed) vel.x = walk_speed;
		}
	});
}

bool Player::on_ground() {
	const auto &lvl = game.get_level();

	if (pos.y >= 0) return true;

	for (int dx = -1; dx <= 1; ++dx) {
		const Vector2 check_point = {
			pos.x + dx,
			pos.y + 0.5f,
		};
		const auto collider = lvl.get_collider(check_point.x, check_point.y);

		if (collider.width == 0 && collider.height == 0) continue;
		if (collider.x >= pos.x + size.x/2 || collider.x + collider.width <= pos.x - size.x/2) continue;

		if (collider.y == pos.y) return true;

		const float bottom_dist_near = collider.y - pos.y;
		const float bottom_dist_far = (collider.y + collider.height) - pos.y;

		const bool inside_bottom = bottom_dist_near <= 0 && bottom_dist_far >= 0;

		if (inside_bottom) return true;
	}

	return false;
}

void Player::resolve_collisions_x() {
	const auto &lvl = game.get_level();

	const float left_edge = pos.x - size.x/2;
	const float right_edge = pos.x + size.x/2;

	for (int dy = -2; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			const Vector2 check_point = {
				pos.x + dx,
				pos.y - 0.5f + dy,
			};
			const auto collider = lvl.get_collider(check_point.x, check_point.y);

			if (collider.width == 0 && collider.height == 0) continue;
			if (collider.y + EPS >= pos.y || collider.y + collider.height - EPS <= pos.y - size.y) continue;

			const float left_dist_near = left_edge - (collider.x + collider.width);
			const float left_dist_far  = left_edge - collider.x;
			const float right_dist_near = collider.x - right_edge;
			const float right_dist_far  = (collider.x + collider.width) - right_edge;

			const bool inside_left = left_dist_near < -EPS && left_dist_far > 0;
			const bool inside_right = right_dist_near < -EPS && right_dist_far > 0;

			const float left_overlap = -left_dist_near; // only if inside_left is true
			const float right_overlap = -right_dist_near; // only if inside_right is true

			if (inside_left || inside_right) {
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
			}
		}
	}
}
void Player::resolve_collisions_y() {
	const auto &lvl = game.get_level();

	if (pos.y > 0) pos.y = 0;

	const float top_edge = pos.y - size.y;
	const float bottom_edge = pos.y;

	for (int dy = -2; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			const Vector2 check_point = {
				pos.x + dx,
				pos.y - 0.5f + dy,
			};
			const auto collider = lvl.get_collider(check_point.x, check_point.y);

			if (collider.width == 0 && collider.height == 0) continue;
			if (collider.x + EPS >= pos.x + size.x/2 || collider.x + collider.width - EPS <= pos.x - size.x/2) continue;

			const float top_dist_near = top_edge - (collider.y + collider.height);
			const float top_dist_far = top_edge - collider.y;
			const float bottom_dist_near = collider.y - bottom_edge;
			const float bottom_dist_far = (collider.y + collider.height) - bottom_edge;

			const bool inside_top = top_dist_near < -EPS && top_dist_far > 0;
			const bool inside_bottom = bottom_dist_near < -EPS && bottom_dist_far > 0;

			const float top_overlap = -top_dist_near; // only if inside_top is true
			const float bottom_overlap = -bottom_dist_near; // only if inside_bottom is true

			if (inside_top || inside_bottom) {
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
			}
		}
	}
}

Vector2 Player::get_pos() const {
	return pos;
}

void Player::update(float dt) {
	if (on_ground()) {
		has_djumped = false;

		vel.y = std::min(0.f, vel.y);

		if (!walking) {
			if (friction * dt >= std::abs(vel.x)) vel.x = 0;
			else if (vel.x > 0) vel.x -= friction * dt;
			else vel.x += friction * dt;
		}
	} else {
		vel.y += game.gravity * dt;
	}

	if (std::abs(vel.y) <= std::abs(vel.x)) {
		pos.x += vel.x * dt;
		resolve_collisions_x();

		pos.y += vel.y * dt;
		resolve_collisions_y();
	} else {
		pos.y += vel.y * dt;
		resolve_collisions_y();

		pos.x += vel.x * dt;
		resolve_collisions_x();
	}

	walking = false;
}
void Player::draw() const {
	DrawRectangleV(
		Vector2{ pos.x - size.x/2, pos.y - size.y },
		size,
		BLACK
	);
}
