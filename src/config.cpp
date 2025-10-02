#include "config.hpp"

#include <iostream>
#include <optional>

#define LIST_OF_ENTRIES CONFIG_LIST_OF_ENTRIES

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

static void trim(std::string &str) {
	auto frst = str.find_first_not_of(" \r\t");
	auto last = str.find_last_not_of(" \r\t");

	if (frst == std::string::npos) str = "";
	else str = str.substr(frst, last - frst + 1);
}

template<typename T>
static std::optional<T> parse(std::string str);
#define PARSER(type) template<> std::optional<type> parse(std::string str)
PARSER(int);
PARSER(WindowState);

Config Config::read(std::istream &stream) {
	Config cfg;
	cfg.had_uninits = false;
#define X(type, name, default, comment) bool name ## _uninit = true;
	LIST_OF_ENTRIES
#undef X
	std::string line;

	while (std::getline(stream, line)) {
		trim(line);
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

#define X(type, name, default, comment) cfg.had_uninits |= name ## _uninit;
	LIST_OF_ENTRIES
#undef X

	return cfg;
}

template<typename T>
static std::string unparse(T &val);
#define WRITER(type) template<> std::string unparse(const type &val)
WRITER(int);
WRITER(WindowState);

void Config::write(std::ostream &stream) const {
#define X(type, name, default, comment) \
	stream << "# " << comment << std::endl; \
	stream << #name << ": " << unparse(this->name) << std::endl;
	LIST_OF_ENTRIES
#undef X
}

PARSER(int) {
	return std::stoi(str);
}
PARSER(WindowState) {
	for (size_t i = 0; i < sizeof(windowstate_strmap)/sizeof(*windowstate_strmap); ++i) {
		if (str == windowstate_strmap[i].str) return windowstate_strmap[i].state;
	}

	return {};
}

WRITER(int) {
	return std::to_string(val);
}
WRITER(WindowState) {
	for (size_t i = 0; i < sizeof(windowstate_strmap)/sizeof(*windowstate_strmap); ++i) {
		if (val == windowstate_strmap[i].state) return windowstate_strmap[i].str;
	}

	std::cerr << "WARNING: Encountered WindowState without corresponding string value!" << std::endl;

	return "";
}
