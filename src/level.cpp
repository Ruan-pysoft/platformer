#include "level.hpp"

Vector2 Level::get_offset() const {
	return { -w/2.0f, -float(h) };
}

Level::Level(const Tile *tilemap, int w, int h, Vector2 player_spawn) : w(w), h(h), player_spawn(player_spawn) {
	tiles.reserve(w*h);
	tiles.assign(tilemap, tilemap + w*h);
}
Vector2 Level::get_player_spawn() const {
	const auto offset = get_offset();
	return { player_spawn.x + offset.x + 0.5f, player_spawn.y + offset.y };
}

Rectangle Level::get_collider(float x, float y) const {
	const auto offset = get_offset();
	const int lvl_x = x - offset.x;
	const int lvl_y = y - offset.y;
	if (lvl_x < 0 || lvl_x >= w || lvl_y < 0 || lvl_y >= h) {
		return { 0, 0, 0, 0 };
	}
	const auto &tile = tiles[lvl_x + lvl_y*w];
	if (!tile.is_solid) {
		return { 0, 0, 0, 0 };
	}
	return { lvl_x + offset.x, lvl_y + offset.y, 1, 1 };
}

void Level::draw() {
	const auto offset = get_offset();
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			DrawRectangle(
				x + offset.x, y + offset.y, 1, 1,
				tiles[x + y*w].color
			);
		}
	}
}
