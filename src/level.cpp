#include "level.hpp"

#include <iostream>
#include <string>

#include "raylib.h"

#include "globals.hpp"
#include "levels_list.hpp"
#include "main_menu.hpp"
#include "player.hpp"
#include "scene.hpp"

void LevelText::draw(const Level &level, const Camera2D &camera) const {
	const Vector2 lvl_offset = level.get_offset();
	const Vector2 world_pos = { pos.x + lvl_offset.x, pos.y + lvl_offset.y - 1 };

	const Vector2 scr_pos = GetWorldToScreen2D(world_pos, camera);
	const int font_size = camera.zoom;

	DrawTextEx(GetFontDefault(), text.c_str(), scr_pos, font_size, 0.0f, color);
}

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
			std::cerr << "WARN: Unknown color " << int(color.r) << ' ' << int(color.g) << ' ' << int(color.b) << ' ' << int(color.a) << " in level image at " << x << ", " << y << std::endl;
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
	level_nr(level_nr), level_time(0), camera_move_time(0), gravity(20)
{
	player->reset(get_player_spawn());

	camera.target = get_player_spawn();
	camera.offset = {
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
	camera.rotation = 0;
	camera.zoom = global::PPU;
}
Level::Level(size_t level_nr, Image image, Vector2 player_spawn)
	: tiles(tilemap_of(image)), w(image.width), h(image.height),
	player(std::make_unique<Player>()), player_spawn { player_spawn.x, h + player_spawn.y },
	level_nr(level_nr), level_time(0), camera_move_time(0), gravity(20)
{
	player->reset(get_player_spawn());

	camera.target = get_player_spawn();
	camera.offset = {
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
	camera.rotation = 0;
	camera.zoom = global::PPU;
}
void Level::add_texts(std::vector<LevelText> texts) {
	for (const auto &text : texts) {
		this->texts.push_back(text);
		this->texts.rbegin()->pos.y += h;
	}
}
Level::~Level() = default;
Vector2 Level::get_player_spawn() const {
	const auto offset = get_offset();

	return { player_spawn.x + offset.x + 0.5f, player_spawn.y + offset.y };
}

void Level::reset() {
	player->reset(get_player_spawn());
}
void Level::full_reset() {
	std::cerr << "Level " << level_nr << " time: " << level_time << std::endl;
	transition.next = Levels::make_level(level_nr);
}
void Level::complete() {
	std::cerr << "Level " << level_nr << " time: " << level_time << std::endl;
	transition.next = Levels::make_level(level_nr+1);
	if (transition.next == nullptr) {
		transition.next = std::make_unique<MainMenu>();
	}
}
void Level::exit() {
	std::cerr << "Level " << level_nr << " time: " << level_time << std::endl;
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
	level_time += dt;

	player->update(*this, dt);

	const auto player_pos = player->get_pos();
	const Vector2 d = {
		player_pos.x - camera.target.x,
		player_pos.y - camera.target.y,
	};
	const Vector2 v = { d.x / camera_follow, d.y / camera_follow };
	camera_move_time -= dt;
	if (d.x*d.x + d.y*d.y > camera_play*camera_play) {
		camera_move_time = camera_min_move_time;

		camera.target.x += v.x * dt;
		camera.target.y += v.y * dt;
	} else if (camera_move_time > 0) {
		camera.target.x += v.x * dt;
		camera.target.y += v.y * dt;
	}

	camera.offset = Vector2 {
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
}
#include <sstream>
void Level::draw() const {
	ClearBackground(RAYWHITE);

	for (const auto &text : texts) {
		text.draw(*this, camera);
	}

	BeginMode2D(camera);

	player->draw();

	const auto offset = get_offset();
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			const Vector2 pos = { x + offset.x, y + offset.y };
			const Vector2 size = { 1, 1 };
			DrawRectangleV(
				pos, size,
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

	std::ostringstream level_time_srm;
	level_time_srm.precision(2);
	level_time_srm << std::fixed << level_time;
	std::string level_time_str = level_time_srm.str();
	const int level_time_str_height = 20;
	const int level_time_str_width = MeasureText(level_time_str.c_str(), level_time_str_height);
	DrawText(level_time_str.c_str(), global::WINDOW_WIDTH - 10 - level_time_str_width, 10, level_time_str_height, BLACK);
}
