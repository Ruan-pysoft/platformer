#pragma once

#include <istream>

enum class WindowState { Windowed, Borderless, Fullscreen };

#define CONFIG_LIST_OF_ENTRIES \
	X(WindowState, window_state, WindowState::Windowed, \
	  "Default window state when the window is opened, one of Windowed, Borderless, or Fullscreen") \
	X(int, window_width, 800, "values below 800 aren't supported") \
	X(int, window_height, 600, "values below 600 aren't supported")

struct Config {
#define X(type, name, default, comment) \
	/* static constexpr const char *name ## _key = #name; */ \
	/* static constexpr const char *name ## _comment = comment; */ \
	type name = default;
	CONFIG_LIST_OF_ENTRIES
#undef X

	bool had_uninits = false;

	static Config read(std::istream &stream);
	void write(std::ostream &stream) const;
};
