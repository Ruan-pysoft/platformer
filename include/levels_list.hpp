#pragma once

#include <memory>
#include <string>
#include <vector>

#include "raylib.h"

#include "level.hpp"

namespace Levels {

struct LevelInfo {
	std::string filename;
	Vector2 spawn;
};

extern const std::vector<LevelInfo> levels;

std::unique_ptr<Level> make_level(size_t idx);

};
