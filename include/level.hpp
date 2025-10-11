#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "raylib.h"

#include "actions.hpp"
#include "overlay.hpp"
#include "player.hpp"
#include "stats.hpp"

/*
 * the main core of the game:
 * handles the 2D array of tiles, the player, and the camera
 */

enum class TileType { Empty, Solid, Danger, Goal, Checkpoint };

struct Tile {
	struct Bounce {
		float top = 0;
		float bottom = 0;
		float side = 0;
	};

	constexpr Tile() : type(TileType::Empty), color(Color { 0, 0, 0, 0 }), in_front(false) {}
	constexpr Tile(Color color) : type(TileType::Solid), color(color), in_front(false) {}
	constexpr Tile(Color color, TileType type) : type(type), color(color), in_front(false) {}
	constexpr Tile(Color color, TileType type, bool in_front) : type(type), color(color), in_front(in_front) {}
	constexpr Tile(Color color, TileType type, bool in_front, Bounce bounce, float friction) : type(type), color(color), in_front(in_front), bounce(bounce), friction(friction) {}

	TileType type;
	Color color;
	bool in_front;
	Bounce bounce;
	float friction = 12;
};

class Level;
struct LevelText {
	std::string text;
	Color color;
	Vector2 pos;

	void draw(const Level &level, const Camera2D &camera) const;
};

class Level {
public:
	enum class State { Active, Paused, WinScreen };
	enum class Change { None, Prev, Next, Reset, MainMenu };

private:
	const std::vector<Tile> tiles;
	int w, h;
	std::unique_ptr<Player> player;
	Vector2 player_spawn;
	Camera2D camera;
	size_t level_nr;
	std::vector<LevelText> texts = {};
	float camera_move_time = 0;
	State state = State::Active;
	ActionOnce::cb_handle_t pause_action;
	Overlay pause_overlay;
	bool has_populated_winscreen = false;
	Overlay win_overlay;
	float frame_acc = 0;
	Stats stats{};
	bool continuous = false;
	std::optional<Vector2> active_checkpoint = {};

	ActionOnce::cb_handle_t reset_action;
	ActionOnce::cb_handle_t next_level_action;

	const float camera_play = 4;
	const float camera_follow = 0.5f;
	const float camera_min_move_time = 0.25;

	Level(
		size_t level_nr, std::vector<Tile> tiles, int w, int h,
		Vector2 player_spawn, bool continuous
	);
public:
	float gravity = 20;
	Change change = Change::None;

	Vector2 get_offset() const;
	int get_level_nr() const;
	const Stats &get_stats() const;

	Level(
		size_t level_nr, const Tile *tilemap, int w, int h,
		Vector2 player_spawn, bool continuous
	);
	Level(
		size_t level_nr, const Tile *tilemap, int w, int h,
		Vector2 player_spawn
	);
	Level(size_t level_nr, Image image, Vector2 player_spawn);
	Level(
		size_t level_nr, Image image, Vector2 player_spawn,
		bool continuous
	);
	void add_texts(std::vector<LevelText> texts);
	~Level();
	Vector2 get_player_spawn() const;

	void respawn_player();
	void display_win_overlay();

	Rectangle get_collider(float x, float y) const;
	Tile get_tile(float x, float y) const;
	void activate_checkpoint(float x, float y);

	void update(float dt);
	void draw() const;
};

namespace Levels {

// pre-defined tiles and colour associations for loading from images

static constexpr Tile floor = Tile(BLACK);
static constexpr Tile air = Tile();
static constexpr Tile wall = Tile(BROWN, TileType::Solid, false, {}, 16);
static constexpr Tile track = Tile({ 95, 63, 0, 255 }, TileType::Solid, false, {}, 32);
static constexpr Tile slime = Tile({ 0, 95, 31, 255 }, TileType::Solid, false, { 0.75, 0.9, 0.5 }, 42);
static constexpr Tile ghost = Tile({ 195, 195, 255, 127 }, TileType::Empty, true);
static constexpr Tile flag = Tile({ 0, 255, 0, 255 }, TileType::Goal);
static constexpr Tile lava = Tile({ 255, 63, 15, 255 }, TileType::Danger);
static constexpr Tile checkpoint = Tile({ 15, 195, 195, 255 }, TileType::Checkpoint);

static constexpr struct {
	Color color;
	Tile tile;
} colormap[] = {
	{ { 0, 0, 0, 255 },       floor },
	{ { 0, 0, 0, 0 },         air },
	{ { 255, 255, 255, 255 }, air },
	{ { 127, 127, 127, 255 }, wall },
	{ { 95, 63, 0, 255 },     track },
	{ { 0, 95, 31, 255 },     slime },
	{ { 195, 195, 255, 255 }, ghost },
	{ { 0, 255, 0, 255 },     flag },
	{ { 255, 0, 0, 255 },     lava },
	{ { 15, 195, 195, 255 },  checkpoint },
};

}
