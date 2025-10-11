#include "config.hpp"

#include <iostream>
#include <optional>

#define LIST_OF_ENTRIES CONFIG_LIST_OF_ENTRIES

// generate a list of string associations to window states
#define WINSTATE(name) { WindowState::name, #name }
constexpr struct {
	WindowState state;
	const char *str;
} windowstate_strmap[] = {
	WINSTATE(Windowed),
	WINSTATE(Borderless),
	WINSTATE(Fullscreen),
};
#undef WINSTATE

// utility function
static void trim(std::string &str) {
	auto frst = str.find_first_not_of(" \r\t");
	auto last = str.find_last_not_of(" \r\t");

	if (frst == std::string::npos) str = "";
	else str = str.substr(frst, last - frst + 1);
}

// define parse function to parse different values from strings
// specialisations are declared here and defined at the end of the file
template<typename T>
static std::optional<T> parse(std::string str);
#define PARSER(type) template<> std::optional<type> parse(std::string str)
PARSER(int);
PARSER(WindowState);

Config Config::read(std::istream &stream) {
	Config cfg;
	cfg.had_uninits = false;
	// keep track on if each field has been loaded from the file with a flag
#define X(type, name, default, comment) bool name ## _uninit = true;
	LIST_OF_ENTRIES
#undef X
	std::string line;

	while (std::getline(stream, line)) {
		trim(line);
		// skip empty or commented lines
		if (line.empty() || line[0] == '#') continue;

		auto pos = line.find(':');
		if (pos == std::string::npos) {
			std::cerr << "Warning: malformed line in config file, skipping!" << std::endl;
			continue;
		}
		std::string key = line.substr(0, pos);
		std::string val = line.substr(pos + 1);
		trim(key);
		trim(val);

		// use X macro to gen code for each config field:
		// if the key matches the field, try to parse it and override the default value
		if (false) {}
#define X(type, name, default, comment) else if (key == #name) { \
			name ## _uninit = false; \
			const auto res = parse<type>(val); \
			if (res.has_value()) { \
				cfg.name = res.value(); \
			} else { \
				std::cerr << "Warning invalid value \"" << val << "\" for key " << key << ", skipping!" << std::endl; \
				name ## _uninit = true; \
			} \
		}
		LIST_OF_ENTRIES
#undef X
		else {
			std::cerr << "Warning: unknown config key " << key << ", skipping!" << std::endl;
		}
	}

	// update the had_uninits flag
#define X(type, name, default, comment) cfg.had_uninits |= name ## _uninit;
	LIST_OF_ENTRIES
#undef X

	return cfg;
}

// define unparse function to write different values to strings
// specialisations are declared here and defined at the end of the file
template<typename T>
static std::string unparse(T &val);
#define WRITER(type) template<> std::string unparse(const type &val)
WRITER(int);
WRITER(WindowState);

void Config::write(std::ostream &stream) const {
	// the writer is much simpler:
	// for each field write the comment, the field name, and the field value
#define X(type, name, default, comment) \
	stream << "# " << comment << std::endl; \
	stream << #name << ": " << unparse(this->name) << std::endl;
	LIST_OF_ENTRIES
#undef X
}

PARSER(int) {
	// integers are handled simply
	return std::stoi(str);
}
PARSER(WindowState) {
	// window states are looked up in the window state array
	for (size_t i = 0; i < sizeof(windowstate_strmap)/sizeof(*windowstate_strmap); ++i) {
		if (str == windowstate_strmap[i].str) return windowstate_strmap[i].state;
	}

	return {};
}

WRITER(int) {
	// integers are stringified simply
	return std::to_string(val);
}
WRITER(WindowState) {
	// window states are looked up in the window state array
	for (size_t i = 0; i < sizeof(windowstate_strmap)/sizeof(*windowstate_strmap); ++i) {
		if (val == windowstate_strmap[i].state) return windowstate_strmap[i].str;
	}

	std::cerr << "WARNING: Encountered WindowState without corresponding string value!" << std::endl;

	return "";
}
