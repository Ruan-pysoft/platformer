#include "player.hpp"

#include <algorithm>

#include "level.hpp"
#include "raylib.h"
#include "util.hpp"

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

Player::Player(Stats &stats) : stats(stats) {
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
	#ifdef DEBUG
	fly_action = Action::Fly.register_cb([this](float) {
		inputs |= MotionInputs::Fly;
	});
	#endif
	suicide_action = Action::Suicide.register_cb([this]() {
		if (!killed) ++this->stats.deaths;
		killed = true;
	});
	slam_action = Action::Slam.register_cb([this](float) {
		inputs |= MotionInputs::Slam;
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

		const TileType type = level.get_tile(check_point.x, check_point.y).type;

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
	const Rectangle player_collider = {
		pos.x - size.x/2, pos.y - size.y,
		size.x, size.y
	};

	for (int dy = -2; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			const Vector2 check_point = {
				pos.x + dx,
				pos.y - 0.5f + dy,
			};
			const auto collider = level.get_collider(check_point.x, check_point.y);
			const auto tile = level.get_tile(check_point.x, check_point.y);

			if (tile.type == TileType::Empty) continue;
			if (collider.width == 0 && collider.height == 0) continue;

			const auto collision = util::collide(player_collider, collider);

			// if there is no x collision, or if the player only
			// slightly overlaps with the block in the y axis,
			// no work to be done
			const bool x_inside = collision.x_touches && collision.dist.x != 0;
			if (!x_inside || std::abs(collision.dist.y) <= EPS) continue;

			switch (tile.type) {
				case TileType::Solid: if (std::abs(collision.dist.x) >= EPS) {
					pos.x = collision.new_pos.x + size.x/2;
					if (collision.dist.x < 0 && vel.x > 0) {
						vel.x *= -tile.bounce.side;
					}
					if (collision.dist.x > 0 && vel.x < 0) {
						vel.x = -tile.bounce.side;
					}
				} break;
				case TileType::Danger: {
					if (!killed) ++stats.deaths;
					killed = true;
				} break;
				case TileType::Goal: {
					level_completed = true;
				} break;
				case TileType::Empty: break;
				case TileType::Checkpoint: {
					level.activate_checkpoint(check_point.x, check_point.y);
				} break;
			}
		}
	}
}
void Player::resolve_collisions_y(Level &level) {
	if (pos.y > 0) pos.y = 0;

	const Rectangle player_collider = {
		pos.x - size.x/2, pos.y - size.y,
		size.x, size.y
	};

	for (int dy = -2; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			const Vector2 check_point = {
				pos.x + dx,
				pos.y - 0.5f + dy,
			};
			const auto collider = level.get_collider(check_point.x, check_point.y);
			const auto tile = level.get_tile(check_point.x, check_point.y);

			if (tile.type == TileType::Empty) continue;
			if (collider.width == 0 && collider.height == 0) continue;

			const auto collision = util::collide(player_collider, collider);

			// if there is no y collision, or if the player only
			// slightly overlaps with the block in the x axis,
			// no work to be done
			const bool y_inside = collision.y_touches && collision.dist.y != 0;
			if (!y_inside || std::abs(collision.dist.x) <= EPS) continue;

			switch (tile.type) {
				case TileType::Solid: if (std::abs(collision.dist.y) >= EPS) {
					pos.y = collision.new_pos.y + size.y;
					if (collision.dist.y < 0 && vel.y > 0) {
						vel.y *= -tile.bounce.top;
					}
					if (collision.dist.y > 0 && vel.y < 0) {
						vel.y *= -tile.bounce.bottom;
					}
				} break;
				case TileType::Danger: {
					if (!killed) ++stats.deaths;
					killed = true;
				} break;
				case TileType::Goal: {
					level_completed = true;
				} break;
				case TileType::Empty: break;
				case TileType::Checkpoint: {
					level.activate_checkpoint(check_point.x, check_point.y);
				} break;
			}
		}
	}
}

Vector2 Player::get_pos(float interp) const {
	if (interp <= 0) return prev_pos;
	if (interp >= 1) return pos;
	return {
		prev_pos.x*(1 - interp) + pos.x*interp,
		prev_pos.y*(1 - interp) + pos.y*interp,
	};
}
void Player::spawn(Vector2 pos) {
	this->pos = pos;
	this->prev_pos = pos;
	this->vel = { 0, 0 };
	this->inputs = MotionInputs::None;
	this->jumpstate = JumpState::DoubleJumped;

	killed = false;
	level_completed = false;
}

void Player::update(Level &level) {
	static int coyote_frames_left = 0;

	const float dt = 1.0f / global::PHYSICS_FPS;

	prev_pos = pos;

	if (killed) {
		level.respawn_player(); return;
	}
	if (level_completed) {
		level.display_win_overlay(); return;
	}

	if (on_ground(level)) {
		jumpstate = JumpState::Grounded;
		coyote_frames_left = coyote_frames;
	} else if (jumpstate == JumpState::Grounded) {
		--coyote_frames_left;
		if (coyote_frames_left <= 0) jumpstate = JumpState::Airborne;
	}

	if (test_input(MotionInputs::Jump) && jumpstate == JumpState::Grounded) {
		++stats.jumps;
		vel.y = -jump_vel;
		jumpstate = JumpState::Airborne;
	} else if (test_input(MotionInputs::DoubleJump) && jumpstate == JumpState::Airborne) {
		++stats.double_jumps;
		vel.y = -jump_vel;
		jumpstate = JumpState::DoubleJumped;
	} else if (test_input(MotionInputs::Slam) && jumpstate != JumpState::Grounded) {
		jumpstate = JumpState::Slamming;
	} else if (test_input(MotionInputs::Slam) && jumpstate == JumpState::Grounded) {
		vel.y = 0;
	}
	if (!test_input(MotionInputs::Slam) && jumpstate == JumpState::Slamming) {
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
	#ifdef DEBUG
	if (test_input(MotionInputs::Fly)) {
		vel.y = std::min(vel.y, -jump_vel / 2.0f);
	}
	#endif

	if (jumpstate == JumpState::Grounded) {
		vel.y = std::min(0.f, vel.y);

		const Vector2 right_below = { pos.x, pos.y + 0.5f };
		const float friction = std::max(std::max(
			level.get_tile(right_below.x, right_below.y).friction,
			level.get_tile(right_below.x - 1, right_below.y).friction
		), level.get_tile(right_below.x + 1, right_below.y).friction
		);
		if (!test_input(MotionInputs::WalkLeft | MotionInputs::WalkRight)) {
			if (friction * dt >= std::abs(vel.x)) vel.x = 0;
			else if (vel.x > 0) vel.x -= friction * dt;
			else vel.x += friction * dt;
		}
	} else {
		#ifdef DEBUG
		if (!test_input(MotionInputs::Fly)) {
			const float scale = jumpstate == JumpState::Slamming ? 2.0f : 1.0f;
			vel.y += level.gravity * scale * dt;
		}
		#else
		const float scale = jumpstate == JumpState::Slamming ? 2.0f : 1.0f;
		vel.y += level.gravity * scale * dt;
		#endif
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

	if (killed) level.respawn_player();
	if (level_completed) level.display_win_overlay();
}
void Player::draw(float interp) const {
	const auto visual_pos = get_pos(interp);
	DrawRectangleV(
		Vector2{ visual_pos.x - size.x/2, visual_pos.y - size.y },
		size,
		BLACK
	);
#ifndef RELEASE
	const std::string y_vel = std::to_string(int(vel.y));
	const std::string x_vel = std::to_string(int(vel.x));
	const auto y_width = MeasureTextEx(GetFontDefault(), y_vel.c_str(), 1, .1);
	const auto x_width = MeasureTextEx(GetFontDefault(), y_vel.c_str(), 1, .1);
	DrawTextEx(GetFontDefault(), x_vel.c_str(), { visual_pos.x - y_width.x/2, visual_pos.y - size.y - 1.25f }, 1, .1, BLACK);
	DrawTextEx(GetFontDefault(), y_vel.c_str(), { visual_pos.x - x_width.x/2, visual_pos.y - size.y - 2.5f }, 1, .1, BLACK);
#endif /* RELEASE */
}
