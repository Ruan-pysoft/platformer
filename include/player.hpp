#pragma once

#include <cstdint>

#include "raylib.h"

#include "actions.hpp"

enum class JumpState {
	Grounded,
	Airborne,
	DoubleJumped,
};

enum class MotionInputs : uint8_t {
	None = 0,
	Jump = (1 << 0),
	DoubleJump = (1 << 1),
	WalkLeft = (1 << 2),
	WalkRight = (1 << 3),
	Fly = (1 << 4),
};

class Level;
class Player {
public:
	struct Stats {
		int jumps = 0;
		int double_jumps = 0;
		int deaths = 0;
		int times_spawned = 0;
	};

private:
	Vector2 prev_pos = { 0, 0 };
	Vector2 pos = { 0, 0 };
	Vector2 vel = { 0, 0 };
	MotionInputs inputs = MotionInputs::None;
	JumpState jumpstate = JumpState::DoubleJumped;
	bool killed = false;
	bool level_completed = false;
	ActionSustain::cb_handle_t jump_action;
	ActionOnce::cb_handle_t double_jump_action;
	ActionSustain::cb_handle_t walk_left_action;
	ActionSustain::cb_handle_t walk_right_action;
	ActionSustain::cb_handle_t fly_action;
	ActionOnce::cb_handle_t suicide_action;
	Stats stats;

	static constexpr Vector2 size = Vector2 { 1.0f, 2.0f };
	static constexpr float jump_vel = 13.25; // old vel is 13, makes some double jumps harder
	static constexpr float walk_acc = 16;
	static constexpr float walk_dec = 32;
	static constexpr float walk_vel = 20;
	static constexpr float friction = 12;
	static constexpr int coyote_frames = 2;

	bool on_ground(Level &level);
	bool test_input(MotionInputs input);

	void resolve_collisions_x(Level &level);
	void resolve_collisions_y(Level &level);
public:
	Player();

	Vector2 get_pos(float interp) const;
	void reset(Vector2 pos);
	Stats get_stats() const;

	void update(Level &level);
	void draw(float interp) const;
};
