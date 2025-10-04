#include "levels_list.hpp"

namespace Levels {

const std::vector<LevelInfo> levels = {
	{ "levels/1.png", { 1, -1 }, {
		{ "Move with the arrow keys or A and D", BLACK, { 2, -4 } }
	} },
	{ "levels/2.png", { 1, -1 }, {
		{ "Jump with W, up arrow, or space", BLACK, { 2, -4 } }
	} },
	{ "levels/3.png", { 1, -1 }, {
		{ "You can also double jump", BLACK, { 2, -4 } }
	} },
	{ "levels/4.png", { 1, -1 }, {
		{ "Red means danger", BLACK, { 2, -4 } }
	} },
	{ "levels/5.png", { 1, -1 }, {
		{ "Double jump to cross the gap", BLACK, { 2, -4 } }
	} },
	{ "levels/level1.png", { 2, -4 }, {} },
	{ "levels/level2.png", { 16, -1 }, {} },
};

std::unique_ptr<Level> make_level(size_t idx) {
	return make_level(idx, false);
}
std::unique_ptr<Level> make_level(size_t idx, bool continuous) {
	if (idx >= levels.size()) return nullptr;

	const auto level_img = LoadImage(levels[idx].filename.c_str());
	auto res = std::make_unique<Level>(
		idx, level_img, levels[idx].spawn, continuous
	);
	res->add_texts(levels[idx].texts);
	UnloadImage(level_img);
	return res;
}

};
