#include "player.hpp"

#include <algorithm>

#include "actions.hpp"
#include "game.hpp"

static const float jump_vel = 12;
static const float walk_acc = 16;
static const float walk_dec = 32;
static const float walk_speed = 20;
static const float friction = 12;
static bool walking = false;

Player::Player(Game &game)
	: game(game),
	  pos { 0, -10 },
	  vel { 0, 0 } {
	Action::Jump.register_cb([this]() {
		if (on_ground()) {
			vel.y = -jump_vel;
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
	return pos.y >= 0;
}

void Player::update(float dt) {
	if (on_ground()) {
		vel.y = std::min(0.f, vel.y);

		if (!walking) {
			if (friction * dt >= std::abs(vel.x)) vel.x = 0;
			else if (vel.x > 0) vel.x -= friction * dt;
			else vel.x += friction * dt;
		}
	} else {
		vel.y += game.gravity * dt;
	}

	pos.x += vel.x * dt;
	pos.y += vel.y * dt;

	if (on_ground()) pos.y = 0;

	walking = false;
}
void Player::draw() const {
	DrawRectangleV(
		Vector2{ pos.x - size.x/2, pos.y - size.y },
		size,
		BLACK
	);
}
