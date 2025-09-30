#include "player.hpp"

#include <algorithm>
//#include <iostream>

#include "level.hpp"

inline constexpr MotionInputs operator|(MotionInputs a, MotionInputs b) {
	return static_cast<MotionInputs>(
		static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
	);
}
inline constexpr MotionInputs &operator|=(MotionInputs &a, MotionInputs b) {
	return a = a | b;
}
inline constexpr bool test_input(MotionInputs mask, MotionInputs input) {
	return (static_cast<uint8_t>(mask) & static_cast<uint8_t>(input)) != 0;
}

static constexpr float EPS = 1.0f / 1024;

Player::Player()  {
	jump_action = Action::Jump.register_cb([this](float) {
		inputs |= MotionInputs::Jump;
	});
	double_jump_action = Action::DoubleJump.register_cb([this]() {
		inputs |= MotionInputs::DoubleJump;
	});
	walk_left_action = Action::Left.register_cb([this](float) {
		inputs |= MotionInputs::WalkLeft;
	});
	walk_right_action = Action::Right.register_cb([this](float) {
		inputs |= MotionInputs::WalkRight;
	});
	fly_action = Action::Fly.register_cb([this](float) {
		inputs |= MotionInputs::Fly;
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
bool Player::test_input(MotionInputs input) {
	return ::test_input(inputs, input);
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
					killed = true;
				} break;
				case TileType::Goal: {
					level_completed = true;
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
					killed = true;
				} break;
				case TileType::Goal: {
					level_completed = true;
				} break;
				case TileType::Empty: break;
			}
		}
	}
}

Vector2 Player::get_pos() const {
	return pos;
}
void Player::reset(Vector2 pos) {
	this->pos = pos;
	this->vel = { 0, 0 };
	this->inputs = MotionInputs::None;
	this->jumpstate = JumpState::DoubleJumped;

	killed = false;
	level_completed = false;
}

void Player::update(Level &level, float dt) {
	static float coyote_time_left = 0;

	if (killed) {
		level.reset(); return;
	}
	if (level_completed) {
		level.complete(); return;
	}

	if (on_ground(level)) {
		jumpstate = JumpState::Grounded;
		coyote_time_left = coyote_time;
	} else if (jumpstate == JumpState::Grounded) {
		coyote_time_left -= dt;
		if (coyote_time_left <= 0) jumpstate = JumpState::Airborne;
	}

	/*if (try_jump) std::cerr << "Trying to jump!" << std::endl;
	if (try_djump) std::cerr << "Trying to double jump!" << std::endl;
	if (walk_left) std::cerr << "Walking left!" << std::endl;
	if (walk_right) std::cerr << "Walking right!" << std::endl;*/

	if (test_input(MotionInputs::Jump) && jumpstate == JumpState::Grounded) {
		vel.y = -jump_vel;
		jumpstate = JumpState::Airborne;
	} else if (test_input(MotionInputs::DoubleJump) && jumpstate == JumpState::Airborne) {
		vel.y = -jump_vel;
		jumpstate = JumpState::DoubleJumped;
	}
	if (test_input(MotionInputs::WalkLeft)) {
		if (vel.x > 0) {
			vel.x -= walk_dec * dt;
		} else if (vel.x > -walk_vel) {
			vel.x -= walk_acc * dt;
			if (vel.x < -walk_vel) vel.x = -walk_vel;
		}
	}
	if (test_input(MotionInputs::WalkRight)) {
		if (vel.x < 0) {
			vel.x += walk_dec * dt;
		} else if (vel.x < walk_vel) {
			vel.x += walk_acc * dt;
			if (vel.x > walk_vel) vel.x = walk_vel;
		}
	}
	if (test_input(MotionInputs::Fly)) {
		vel.y = std::min(vel.y, -jump_vel / 2.0f);
	}

	if (jumpstate == JumpState::Grounded) {
		vel.y = std::min(0.f, vel.y);

		if (!test_input(MotionInputs::WalkLeft | MotionInputs::WalkRight)) {
			if (friction * dt >= std::abs(vel.x)) vel.x = 0;
			else if (vel.x > 0) vel.x -= friction * dt;
			else vel.x += friction * dt;
		}
	} else {
		if (!test_input(MotionInputs::Fly)) {
			vel.y += level.gravity * dt;
		}
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

	inputs = MotionInputs::None;

	if (killed) level.reset();
	if (level_completed) level.complete();
}
void Player::draw() const {
	DrawRectangleV(
		Vector2{ pos.x - size.x/2, pos.y - size.y },
		size,
		BLACK
	);
}
