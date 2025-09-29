#include "levels_list.hpp"

#include <cassert>

namespace Levels {

const std::vector<LevelInfo> levels = {
	{ "levels/level1.png", Levels::lvl1_spawn },
	{ "levels/level2.png", Levels::lvl2_spawn },
};

std::unique_ptr<Level> make_level(size_t idx) {
	assert(idx >= 0); // size_t should be unsigned?

	if (idx >= levels.size()) return nullptr;

	const auto level_img = LoadImage(levels[idx].filename.c_str());
	return std::make_unique<Level>(idx, level_img, levels[idx].spawn);
	UnloadImage(level_img);
}

};
