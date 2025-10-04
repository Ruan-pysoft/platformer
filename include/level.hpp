#pragma once

#include <memory>
#include <string>
#include <vector>

#include "raylib.h"

#include "actions.hpp"
#include "overlay.hpp"
#include "player.hpp"

enum class TileType { Empty, Solid, Danger, Goal };

struct Tile {
	constexpr Tile() : type(TileType::Empty), color(Color { 0, 0, 0, 0 }) {}
	constexpr Tile(Color color) : type(TileType::Solid), color(color) {}
	constexpr Tile(Color color, TileType type) : type(type), color(color) {}

	TileType type;
	Color color;
};

class Level;
struct LevelText {
	std::string text;
	Color color;
	Vector2 pos;

	void draw(const Level &level, const Camera2D &camera) const;
};

enum class LevelState { Active, Paused, WinScreen };
enum class LevelChange { None, Prev, Next, Reset, MainMenu };

class Level {
public:
	struct Stats {
		int level_ticks = 0;

		void accumulate(const Stats &other) {
			level_ticks += other.level_ticks;
		}
	};

private:
	const std::vector<Tile> tiles;
	int w, h;
	std::unique_ptr<Player> player;
	Vector2 player_spawn;
	Camera2D camera;
	size_t level_nr;
	std::vector<LevelText> texts = {};
	float camera_move_time = 0;
	LevelState state = LevelState::Active;
	ActionOnce::cb_handle_t pause_action;
	Overlay pause_overlay;
	Overlay win_overlay;
	float frame_acc = 0;
	Stats stats {};
	bool continuous = false;

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
	LevelChange change = LevelChange::None;

	Vector2 get_offset() const;
	int get_level_nr() const;
	Stats get_stats() const;
	Player::Stats get_player_stats() const;

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
	TileType get_tile_type(float x, float y) const;

	void update(float dt);
	void draw() const;
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

}
