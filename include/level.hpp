#pragma once

#include <vector>

#include "raylib.h"

struct Tile {
	constexpr Tile() : color(Color { 0, 0, 0, 0 }), is_solid(false) {}
	constexpr Tile(Color color) : color(color), is_solid(true) {}
	constexpr Tile(Color color, bool is_solid) : color(color), is_solid(is_solid) {}

	Color color;
	bool is_solid;
};

class Level {
	std::vector<Tile> tiles;
	int w, h;
	Vector2 player_spawn;

	Vector2 get_offset() const;
public:
	Level(const Tile *tilemap, int w, int h, Vector2 player_spawn);
	Vector2 get_player_spawn() const;

	Rectangle get_collider(float x, float y) const;

	void draw();
};

namespace Levels {

static constexpr Tile floor = Tile(BLACK);
static constexpr Tile air = Tile();
static constexpr Tile wall = Tile(BROWN);
static constexpr Tile ghost = Tile(Color { 195, 195, 255, 127 }, false);

const int lvl1_width = 20;
const int lvl1_height = 5;
const Tile lvl1_tileset[lvl1_width * lvl1_height] = {
	wall,  air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   wall,
	wall,  air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   air,   wall,
	wall,  air,   air,   air,   air,   air,   air,   air,   air,   ghost, ghost, air,   air,   air,   air,   air,   air,   air,   air,   wall,
	wall,  air,   air,   air,   air,   air,   air,   air,   air,   ghost, ghost, air,   air,   air,   air,   air,   air,   air,   air,   wall,
	floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor, floor,
};
const Vector2 lvl1_spawn = { 1, 2 };

}
