#include "level.hpp"

#include <iostream>
#include <string>

#include "globals.hpp"
#include "levels_list.hpp"
#include "main_menu.hpp"
#include "player.hpp"
#include "scene.hpp"

static constexpr float camera_play = 4;
static constexpr float camera_follow = 0.5f;

Vector2 Level::get_offset() const {
	return { -w/2.0f, -float(h) };
}

static std::vector<Tile> tilemap_of(Image image) {
	std::vector<Tile> tiles;

	tiles.reserve(image.width*image.height);
	using namespace Levels;
	for (int y = 0; y < image.height; ++y) {
		for (int x = 0; x < image.width; ++x) {
			const auto color = GetImageColor(image, x, y);
			for (int i = 0; i < int(sizeof(colormap)/sizeof(*colormap)); ++i) {
				const auto r = colormap[i].color.r == color.r;
				const auto g = colormap[i].color.g == color.g;
				const auto b = colormap[i].color.b == color.b;
				const auto a = colormap[i].color.a == color.a;
				if (r && g && b && a) {
					tiles.push_back(colormap[i].tile);
					goto matched_color;
				}
			}
			std::cerr << "WARN: Unknown color " << color.r << ' ' << color.g << ' ' << color.b << ' ' << color.a << " in level image" << std::endl;
			tiles.push_back(air);
		matched_color:;
		}
	}

	return tiles;
}

Level::Level(size_t level_nr, const Tile *tilemap, int w, int h, Vector2 player_spawn)
	: tiles(tilemap, tilemap + w*h), w(w), h(h),
	player(std::make_unique<Player>()),
	player_spawn { player_spawn.x, h + player_spawn.y },
	level_nr(level_nr), gravity(20)
{
	player->reset(get_player_spawn());

	camera.target = get_player_spawn();
	camera.offset = {
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
	camera.rotation = 0;
	camera.zoom = global::PPU - 1;
}
Level::Level(size_t level_nr, Image image, Vector2 player_spawn)
	: tiles(tilemap_of(image)), w(image.width), h(image.height),
	player(std::make_unique<Player>()), player_spawn { player_spawn.x, h + player_spawn.y },
	level_nr(level_nr), gravity(20)
{
	player->reset(get_player_spawn());

	camera.target = get_player_spawn();
	camera.offset = {
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
	camera.rotation = 0;
	camera.zoom = global::PPU - 1;
}
Level::~Level() = default;
Vector2 Level::get_player_spawn() const {
	const auto offset = get_offset();
	return { player_spawn.x + offset.x + 0.5f, player_spawn.y + offset.y };
}

void Level::reset() {
	player->reset(get_player_spawn());
}
void Level::complete() {
	transition.next = Levels::make_level(level_nr+1);
	if (transition.next == nullptr) {
		transition.next = std::make_unique<MainMenu>();
	}
}
void Level::exit() {
	transition.next = std::make_unique<MainMenu>();
}

Rectangle Level::get_collider(float x, float y) const {
	const auto offset = get_offset();
	const int lvl_x = x - offset.x;
	const int lvl_y = y - offset.y;
	if (lvl_x < 0 || lvl_x >= w || lvl_y < 0 || lvl_y >= h) {
		return { 0, 0, 0, 0 };
	}
	const auto &tile = tiles[lvl_x + lvl_y*w];
	if (tile.type == TileType::Empty) {
		return { 0, 0, 0, 0 };
	}
	return { lvl_x + offset.x, lvl_y + offset.y, 1, 1 };
}
TileType Level::get_tile_type(float x, float y) const {
	const auto offset = get_offset();
	const int lvl_x = x - offset.x;
	const int lvl_y = y - offset.y;
	if (lvl_x < 0 || lvl_x >= w || lvl_y < 0 || lvl_y >= h) {
		return TileType::Empty;
	}
	return tiles[lvl_x + lvl_y*w].type;
}

void Level::update(float dt) {
	player->update(*this, dt);

	const auto player_pos = player->get_pos();
	const Vector2 d = {
		player_pos.x - camera.target.x,
		player_pos.y - camera.target.y,
	};
	if (std::abs(d.x) > camera_play) {
		const float v = d.x / camera_follow;
		camera.target.x += v * dt;
	}
	if (std::abs(d.y) > camera_play) {
		const float v = d.y / camera_follow;
		camera.target.y += v * dt;
	}

	camera.offset = Vector2 {
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
}
void Level::draw() const {
	ClearBackground(RAYWHITE);

	BeginMode2D(camera);

	player->draw();

	const auto offset = get_offset();
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			DrawRectangle(
				x + offset.x, y + offset.y, 1, 1,
				tiles[x + y*w].color
			);
		}
	}

	EndMode2D();

	std::string level_display = "Level: ";
	level_display += std::to_string(level_nr + 1);
	level_display += " / ";
	level_display += std::to_string(Levels::levels.size());

	const int level_display_height = 20;
	DrawText(level_display.c_str(), 10, 10, level_display_height, BLACK);
}
