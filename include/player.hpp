#pragma once

#include <cstdint>

#include "raylib.h"

#include "actions.hpp"
#include "stats.hpp"

// class for handling physics simulation and rendering of the player

enum class JumpState {
	Grounded,
	Airborne,
	DoubleJumped,
	Slamming,
};

enum class MotionInputs : uint8_t {
	None = 0,
	Jump = (1 << 0),
	DoubleJump = (1 << 1),
	WalkLeft = (1 << 2),
	WalkRight = (1 << 3),
	#ifdef DEBUG
	Fly = (1 << 4),
	#endif
	Slam = (1 << 5),
};

class Level;
class Player {
	Vector2 prev_pos = { 0, 0 };
	Vector2 pos = { 0, 0 };
	Vector2 vel = { 0, 0 };
	MotionInputs inputs = MotionInputs::None;
	JumpState jumpstate = JumpState::DoubleJumped;
	bool killed = false;
	bool level_completed = false;
	ActionSustain::cb_handle_t jump_action;
	ActionOnce::cb_handle_t double_jump_action;
	ActionSustain::cb_handle_t slam_action;
	ActionSustain::cb_handle_t walk_left_action;
	ActionSustain::cb_handle_t walk_right_action;
	#ifdef DEBUG
	ActionSustain::cb_handle_t fly_action;
	#endif
	ActionOnce::cb_handle_t suicide_action;
	Stats &stats;

	static constexpr Vector2 size = Vector2 { 1.0f, 2.0f };
	static constexpr float jump_vel = 13; // set to 13.25 for much easier 8-block double jumps
	static constexpr float walk_acc = 16;
	static constexpr float walk_dec = 32;
	static constexpr float walk_vel = 20;
	static constexpr int coyote_frames = 2;

	bool on_ground(Level &level);
	bool test_input(MotionInputs input);

	void resolve_collisions_x(Level &level);
	void resolve_collisions_y(Level &level);
public:
	Player(Stats &stats);

	Vector2 get_pos(float interp) const;
	void spawn(Vector2 pos);

	void update(Level &level);
	void draw(float interp) const;
};
