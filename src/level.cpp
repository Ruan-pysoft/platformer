#include "level.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "raylib.h"

#include "actions.hpp"
#include "globals.hpp"
#include "levels_list.hpp"
#include "player.hpp"

void LevelText::draw(const Level &level, const Camera2D &camera) const {
	const Vector2 lvl_offset = level.get_offset();
	const Vector2 world_pos = { pos.x + lvl_offset.x, pos.y + lvl_offset.y - 1 };

	const Vector2 scr_pos = GetWorldToScreen2D(world_pos, camera);
	const int font_size = camera.zoom;

	// from raylib/src/rtext.c:1195
	const float spacing = font_size / 10.0f;

	DrawTextEx(GetFontDefault(), text.c_str(), scr_pos, font_size, spacing, color);
}

Level::Level(size_t level_nr, std::vector<Tile> tiles, int w, int h,
	     Vector2 player_spawn, bool continuous)
: tiles(tiles), w(w), h(h), player(std::make_unique<Player>(stats)),
  player_spawn { player_spawn.x, h + player_spawn.y }, level_nr(level_nr),
  pause_overlay(), win_overlay(), continuous(continuous)
{
	player->spawn(get_player_spawn());

	camera.target = get_player_spawn();
	camera.offset = {
		global::WINDOW_WIDTH / 2.0f,
		global::WINDOW_HEIGHT / 2.0f,
	};
	camera.rotation = 0;
	camera.zoom = global::PPU;

	pause_action = Action::Pause.register_cb([this]() {
		if (state == Level::State::Paused) {
			state = Level::State::Active;
		} else if (state == Level::State::Active) {
			state = Level::State::Paused;
		}
	});

	Text pause_text = { "", 50, { 0, 12.5 }, true, BLACK };
	pause_text.text += "Paused - Level ";
	pause_text.text += std::to_string(level_nr + 1);
	pause_overlay.add_text(pause_text);
	pause_overlay.add_button({
		[this]() {
			state = Level::State::Active;
		},
		GuiBox::floating_x({ -137.5f, 100 }, { 250, 75 }), "RESUME"
	});
	pause_overlay.add_button({
		[this]() {
			change = Level::Change::MainMenu;
		},
		GuiBox::floating_x({ 137.5f, 100 }, { 250, 75 }), "MAIN MENU"
	});
	const auto pause_prev_box = GuiBox::floating_x({ 0, 200 }, { 525, 75 });
	const auto pause_prev_txt = "PREVIOUS LEVEL";
	if (!continuous && level_nr == 0) {
		pause_overlay.add_button({
			[]() {}, pause_prev_box, pause_prev_txt,
			Button::DEFAULT_TEXT_SIZE, LIGHTGRAY, LIGHTGRAY,
			Button::DEFAULT_TEXT_COLOR
		});
	} else if (!continuous) {
		pause_overlay.add_button({
			[this]() {
				change = Level::Change::Prev;
			},
			pause_prev_box, pause_prev_txt
		});
	}
	pause_overlay.add_button({
		[this]() {
			if (this->continuous && state == Level::State::WinScreen) {
				return;
			}
			++this->stats.restarts;
			change = Level::Change::Reset;
		},
		GuiBox::floating_x(
			{ 0, continuous ? 200.0f : 300.0f }, { 525, 75 }
		), "RESTART LEVEL"
	});

	win_overlay.add_text({ "Level completed", 50, { 0, 12.5 }, true, BLACK });
	win_overlay.add_text({ "", 24, { 0, 75 }, true, BLACK });
	win_overlay.add_text({ "", 24, { 0, 100 }, true, BLACK });


	if (continuous) {
		win_overlay.add_button({
			[this]() {
				change = Level::Change::Next;
			},
			GuiBox::floating_x({ -137.5f, 150 }, { 250, 75 }), "CONTINUE"
		});
	} else {
		win_overlay.add_button({
			[this]() {
				change = Level::Change::Next;
			},
			GuiBox::floating_x({ -137.5f, 150 }, { 250, 75 }), "PROCEED"
		});
	}
	win_overlay.add_button({
		[this]() {
			change = Level::Change::MainMenu;
		},
		GuiBox::floating_x({ 137.5f, 150 }, { 250, 75 }), "MAIN MENU"
	});
	if (!continuous) {
		const auto win_prev_box = GuiBox::floating_x({ 0, 250 }, { 525, 75 });
		const auto win_prev_txt = "PREVIOUS LEVEL";
		if (level_nr == 0) {
			win_overlay.add_button({
				[]() {}, win_prev_box, win_prev_txt,
				Button::DEFAULT_TEXT_SIZE, LIGHTGRAY, LIGHTGRAY,
				Button::DEFAULT_TEXT_COLOR
			});
		} else {
			win_overlay.add_button({
				[this]() {
					change = Level::Change::Prev;
				},
				win_prev_box, win_prev_txt
			});
		}
		win_overlay.add_button({
			[this]() {
				change = Level::Change::Reset;
			},
			GuiBox::floating_x({ 0, 350 }, { 525, 75 }), "RESTART LEVEL"
		});
	}

	reset_action = Action::Reset.register_cb([this]() {
		++this->stats.restarts;
		change = Level::Change::Reset;
	});
	next_level_action = Action::NextLevel.register_cb([this]() {
		if (state == Level::State::WinScreen) {
			change = Level::Change::Next;
		}
	});
}

Vector2 Level::get_offset() const {
	return { -w/2.0f, -float(h) };
}
int Level::get_level_nr() const {
	return level_nr;
}
const Stats &Level::get_stats() const {
	return stats;
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

Level::Level(size_t level_nr, const Tile *tilemap, int w, int h,
	     Vector2 player_spawn)
: Level(level_nr, tilemap, w, h, player_spawn, false)
{ }
Level::Level(size_t level_nr, const Tile *tilemap, int w, int h,
	     Vector2 player_spawn, bool continuous)
: Level(level_nr, { tilemap, tilemap + w*h }, w, h, player_spawn, continuous)
{ }
Level::Level(size_t level_nr, Image image, Vector2 player_spawn)
: Level(level_nr, image, player_spawn, false)
{ }
Level::Level(size_t level_nr, Image image, Vector2 player_spawn,
	     bool continuous)
: Level(level_nr, tilemap_of(image), image.width, image.height, player_spawn, continuous)
{ }
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

void Level::respawn_player() {
	player->spawn(get_player_spawn());
}
void Level::display_win_overlay() {
	state = Level::State::WinScreen;
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
Tile Level::get_tile(float x, float y) const {
	const auto offset = get_offset();
	const int lvl_x = x - offset.x;
	const int lvl_y = y - offset.y;
	if (lvl_x < 0 || lvl_x >= w || lvl_y < 0 || lvl_y >= h) {
		return Tile();
	}
	return tiles[lvl_x + lvl_y*w];
}
void Level::activate_checkpoint(float x, float y) {
	const auto offset = get_offset();
	const int lvl_x = x - offset.x;
	const int lvl_y = y - offset.y;
	if (lvl_x < 0 || lvl_x >= w || lvl_y < 0 || lvl_y >= h) {
		return;
	}
	active_checkpoint = { float(lvl_x), float(lvl_y) };
	player_spawn = { float(lvl_x), lvl_y + 1.f };
}

void Level::update(float dt) {
	switch (state) {
		case Level::State::Paused: {
			pause_overlay.update(dt);
		} break;
		case Level::State::WinScreen: {
			if (!has_populated_winscreen) {
				has_populated_winscreen = true;

				PBFile pbs_file = PBFile::load();
				const std::string key = std::to_string(level_nr);
				auto pb = pbs_file.get(key);

				const bool new_pb = pb == nullptr || stats.better_than(*pb);

				Text *stats_text = win_overlay.get_text(1);
				Text *pb_text = win_overlay.get_text(2);

				const std::string stats_label = "Time / Jumps / Deaths: ";
				std::string stats_value;

				const unsigned seconds = stats.time / global::PHYSICS_FPS;
				const unsigned frames = stats.time % global::PHYSICS_FPS;
				stats_value += std::to_string(seconds);
				stats_value += ";";
				if (frames < 10) stats_value += "0";
				stats_value += std::to_string(frames);

				stats_value += " / ";

				stats_value += std::to_string(stats.total_jumps());

				stats_value += " / ";

				stats_value += std::to_string(stats.deaths);

				stats_text->text = stats_label + stats_value;
				if (new_pb) {
					stats_text->text += " (New PB!)";
				}

				const std::string pb_label = "Personal Best: ";
				std::string pb_value;

				if (pb == nullptr) {
					pb_value = "N/A";
				} else {
					const unsigned seconds = pb->time / global::PHYSICS_FPS;
					const unsigned frames = pb->time % global::PHYSICS_FPS;
					pb_value += std::to_string(seconds);
					pb_value += ";";
					if (frames < 10) pb_value += "0";
					pb_value += std::to_string(frames);

					pb_value += " / ";

					pb_value += std::to_string(pb->total_jumps());

					pb_value += " / ";

					pb_value += std::to_string(pb->deaths);
				}

				pb_text->text = pb_label + pb_value;

				if (new_pb) {
					pbs_file.set(key, stats);
					pbs_file.save();
				}
			}

			win_overlay.update(dt);
		} break;
		case Level::State::Active: break;
	}

	if (state != Level::State::Active) return;

	frame_acc += dt;
	const bool physics_tick = frame_acc >= 1.0f/global::PHYSICS_FPS;
	if (physics_tick) {
		while (frame_acc >= 1.0f/global::PHYSICS_FPS) {
			frame_acc -= 1.0f/global::PHYSICS_FPS;
		}
		++stats.time;

		player->update(*this);
	}

	const auto player_pos = player->get_pos(frame_acc * global::PHYSICS_FPS);
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
void Level::draw() const {
	ClearBackground(RAYWHITE);

	for (const auto &text : texts) {
		text.draw(*this, camera);
	}

	BeginMode2D(camera);

	const auto offset = get_offset();
	std::vector<std::pair<Rectangle, Color>> draw_after {};

	const float viewport_width = global::WINDOW_WIDTH / camera.zoom;
	const float viewport_height = global::WINDOW_HEIGHT / camera.zoom;
	const float viewport_left = camera.target.x + - offset.x - viewport_width/2.f;
	const float viewport_right = camera.target.x - offset.x + viewport_width/2.f;
	const float viewport_top = camera.target.y - offset.y - viewport_height/2.f;
	const float viewport_bottom = camera.target.y - offset.y + viewport_height/2.f;

	const float min_fade_dist = 1;
	const float max_fade_dist = 5;
	const float min_units = 32;
	const float max_units = 64;
	const float units = std::min(viewport_right - viewport_left, viewport_bottom - viewport_top);
	const float fade_lerp = std::max(0.f, std::min(1.f, (units - min_units)/(max_units - min_units)));
	const float fade_dist = min_fade_dist + fade_lerp*(max_fade_dist - min_fade_dist);

	const int y_min = std::max(viewport_top, 0.f);
	const int y_max = std::min(viewport_bottom, float(h-1));
	const int x_min = std::max(viewport_left, 0.f);
	const int x_max = std::min(viewport_right, float(w-1));

	for (int y = y_min; y <= y_max; ++y) {
		for (int x = x_min; x <= x_max; ++x) {
			const Vector2 pos = { x + offset.x, y + offset.y };
			const Vector2 size = { 1, 1 };

			const float y_dist = std::min(y + size.y - viewport_top, viewport_bottom - y);
			const float x_dist = std::min(x + size.x - viewport_left, viewport_right - x);
			const float dist = std::min(x_dist, y_dist);

			const float factor = dist < fade_dist ? dist / fade_dist : 1;
			const float adj = (1 - factor)/2;
			const auto color = tiles[x + y*w].color;

			if (color.a == 0) continue; // don't draw invisible tiles

			const Rectangle rect = {
				pos.x + size.x*adj, pos.y + size.y*adj,
				size.x*factor, size.y*factor
			};

			if (tiles[x + y*w].in_front) {
				draw_after.push_back(std::make_pair(rect, color));
			} else {
				DrawRectangleRec(rect, color);
			}
		}
	}

	if (active_checkpoint.has_value()) {
		DrawPoly({
			offset.x + active_checkpoint->x + 0.5f,
			offset.y + active_checkpoint->y + 0.5f,
		}, 4, 0.5f, 0, { 127, 255, 127, 195 });
	}

	player->draw(frame_acc * global::PHYSICS_FPS);

	for (const auto &e : draw_after) {
		DrawRectangleRec(e.first, e.second);
	}

	EndMode2D();

	std::string level_display = "Level: ";
	level_display += std::to_string(level_nr + 1);
	level_display += " / ";
	level_display += std::to_string(Levels::levels.size());

	const int level_display_height = 20;
	DrawText(level_display.c_str(), 10, 10, level_display_height, BLACK);

	std::string level_time_str = "";
	const int seconds = stats.time / global::PHYSICS_FPS;
	const int frames = stats.time % global::PHYSICS_FPS;
	level_time_str += std::to_string(seconds);
	level_time_str += ";";
	if (frames < 10) level_time_str += "0";
	level_time_str += std::to_string(frames);
	const int level_time_str_height = 20;
	const int level_time_str_width = MeasureText(level_time_str.c_str(), level_time_str_height);
	DrawText(level_time_str.c_str(), global::WINDOW_WIDTH - 10 - level_time_str_width, 10, level_time_str_height, BLACK);

	switch (state) {
		case Level::State::Paused: {
			pause_overlay.draw();
		} break;
		case Level::State::WinScreen: {
			win_overlay.draw();
		} break;
		case Level::State::Active: break;
	}
}
