#pragma once

#include <string>
#include <utility>
#include <vector>

#include "raylib.h"

#include "actions.hpp"
#include "gui.hpp"
#include "scene.hpp"

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

class Player;
class Level : public Scene {
	class Overlay {
		Level &level;

		std::vector<std::pair<Text, bool>> text;
		std::vector<Button> buttons;
	public:
		Overlay(Level &level);

		Overlay &add_text(Text text, bool centered);
		Overlay &add_button(Button button);

		Text *get_text(size_t ix);
		Button *get_button(size_t ix);

		void update(float dt);
		void draw() const;
	};

	const std::vector<Tile> tiles;
	int w, h;
	std::unique_ptr<Player> player;
	Vector2 player_spawn;
	Camera2D camera;
	size_t level_nr;
	std::vector<LevelText> texts = {};
	int level_ticks = 0;
	float camera_move_time = 0;
	LevelState state = LevelState::Active;
	ActionOnce::cb_handle_t pause_action;
	Overlay pause_overlay;
	Overlay win_overlay;
	float frame_acc = 0;

	const float camera_play = 4;
	const float camera_follow = 0.5f;
	const float camera_min_move_time = 0.25;

	Level(size_t level_nr, std::vector<Tile> tiles, int w, int h, Vector2 player_spawn);
public:
	float gravity = 20;

	Vector2 get_offset() const;

	Level(size_t level_nr, const Tile *tilemap, int w, int h, Vector2 player_spawn);
	Level(size_t level_nr, Image image, Vector2 player_spawn);
	void add_texts(std::vector<LevelText> texts);
	~Level();
	Vector2 get_player_spawn() const;

	void reset();
	void full_reset();
	void complete();
	void exit();

	Rectangle get_collider(float x, float y) const;
	TileType get_tile_type(float x, float y) const;

	void update(float dt) override;
	void draw() const override;
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
