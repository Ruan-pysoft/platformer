#include "player.hpp"

#include <iostream>

#include "game.hpp"
#include "globals.hpp"

Player::Player(Game &game)
	: game(game),
	  pos(global::worldspace(Vector2 {
		global::WINDOW_WIDTH / 2.0f, global::WINDOW_HEIGHT / 2.0f
	  })),
	  vel(Vector2 { 0, 0 }) { }

bool Player::on_ground() {
	return pos.y <= 0;
}

void Player::update(float dt) {
	if (on_ground()) {
		pos.y = 0;
		vel.y = 0;
	} else {
		vel.y -= game.gravity * dt;
	}

	pos.x += vel.x * dt;
	pos.y += vel.y * dt;
}
void Player::draw() {
	const Vector2 topLeft = Vector2 { pos.x - size.x / 2, pos.y + size.y };
	const Vector2 psize = Vector2 { global::pixels(size.x), global::pixels(size.y) };
	DrawRectangleV(
		global::displayspace(topLeft),
		psize,
		BLACK
	);
}
