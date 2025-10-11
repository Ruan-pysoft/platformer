#pragma once

#include <memory>
#include <string>
#include <vector>

#include "raylib.h"

#include "level.hpp"

/*
 * defines level data for all the playable levels
 * provides some utility functions for loading levels too
 */

namespace Levels {

struct LevelInfo {
	std::string filename;
	Vector2 spawn;
	std::vector<LevelText> texts;
};

extern const std::vector<LevelInfo> levels;

std::unique_ptr<Level> make_level(size_t idx);
std::unique_ptr<Level> make_level(size_t idx, bool continuous);

};
