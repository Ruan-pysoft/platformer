#include "player.hpp"

#include "game.hpp"

Player::Player(Game &game)
	: game(game),
	  pos { 0, -10 },
	  vel { 0, 0 } {
}

bool Player::on_ground() {
	return pos.y >= 0;
}

void Player::update(float dt) {
	pos.x += vel.x * dt;
	pos.y += vel.y * dt;

	if (on_ground()) {
		pos.y = 0;
		vel.y = 0;
	} else {
		vel.y += game.gravity * dt;
	}
}
void Player::draw() const {
	DrawRectangleV(
		Vector2{ pos.x - size.x/2, pos.y - size.y },
		size,
		BLACK
	);
}
