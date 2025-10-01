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
	Vector2 pos = { 0, 0 };
	Vector2 vel = { 0, 0 };
	MotionInputs inputs = MotionInputs::None;
	JumpState jumpstate = JumpState::DoubleJumped;
	int deaths = 0;
	bool killed = false;
	bool level_completed = false;
	ActionSustain::cb_handle_t jump_action;
	ActionOnce::cb_handle_t double_jump_action;
	ActionSustain::cb_handle_t walk_left_action;
	ActionSustain::cb_handle_t walk_right_action;
	ActionSustain::cb_handle_t fly_action;
	ActionOnce::cb_handle_t suicide_action;

	static constexpr Vector2 size = Vector2 { 1.0f, 2.0f };
	static constexpr float jump_vel = 13;
	static constexpr float walk_acc = 16;
	static constexpr float walk_dec = 32;
	static constexpr float walk_vel = 20;
	static constexpr float friction = 12;
	static constexpr float coyote_time = 1.0f / 16;

	bool on_ground(Level &level);
	bool test_input(MotionInputs input);

	void resolve_collisions_x(Level &level);
	void resolve_collisions_y(Level &level);
public:
	Player();

	Vector2 get_pos() const;
	void reset(Vector2 pos);
	int get_deaths() const;

	void update(Level &level, float dt);
	void draw() const;
};
