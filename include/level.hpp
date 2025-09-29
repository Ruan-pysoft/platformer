#pragma once

#include <vector>

#include "raylib.h"

#include "scene.hpp"

enum class TileType { Empty, Solid, Danger, Goal };

struct Tile {
	constexpr Tile() : type(TileType::Empty), color(Color { 0, 0, 0, 0 }) {}
	constexpr Tile(Color color) : type(TileType::Solid), color(color) {}
	constexpr Tile(Color color, TileType type) : type(type), color(color) {}

	TileType type;
	Color color;
};

class Player;
class Level : public Scene {
	const std::vector<Tile> tiles;
	int w, h;
	Player &player;
	Vector2 player_spawn;
	SceneAction action;
	Camera2D camera;

	Vector2 get_offset() const;
public:
	float gravity;

	Level(const Tile *tilemap, int w, int h, Vector2 player_spawn);
	Level(Image image, Vector2 player_spawn);
	Vector2 get_player_spawn() const;

	void reset();
	void complete();
	void exit();

	Rectangle get_collider(float x, float y) const;

	void update(float dt) override;
	void draw() const override;
	SceneAction get_action() const override;
};

namespace Levels {

static constexpr Tile floor = Tile(BLACK);
static constexpr Tile air = Tile();
static constexpr Tile wall = Tile(BROWN);
static constexpr Tile ghost = Tile({ 195, 195, 255, 127 }, TileType::Empty);
static constexpr Tile flag = Tile({ 0, 255, 0, 255 }, TileType::Goal);
static constexpr Tile lava = Tile({ 255, 63, 15, 255 }, TileType::Danger);

static constexpr struct {
	Color color;
	Tile tile;
} colormap[] = {
	{ { 0, 0, 0, 255 },       floor },
	{ { 0, 0, 0, 0 },         air },
	{ { 127, 127, 127, 255 }, wall },
	{ { 195, 195, 255, 255 }, ghost },
	{ { 0, 255, 0, 255 },     flag },
	{ { 255, 0, 0, 255 },     lava },
};

const Vector2 lvl1_spawn = { 2, -4 };

}
