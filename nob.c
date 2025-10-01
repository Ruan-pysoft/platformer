// see https://github.com/tsoding/nob.h/blob/main/how_to/001_basic_usage/nob.c

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOB_WARN_DEPRECATED
#include "nob.h"

#define BUILD_DIR "build/"
#define SRC_DIR "src/"
#define INC_DIR "include/"

const char outfile[] = BUILD_DIR"game";

#define HPP(header) const char *const header ## _hpp = INC_DIR #header ".hpp"

HPP(actions);
HPP(game);
HPP(gui);
HPP(globals);
HPP(input_manager);
HPP(level);
HPP(level_select);
HPP(levels_list);
HPP(main_menu);
HPP(player);
HPP(scene);

#define DEPS(of) const char *const of ## _deps[] =
#define NO_DEPS(of) const char *const of ## _deps[] = { of ## _hpp }

DEPS(main) {
	actions_hpp, input_manager_hpp, game_hpp,
	globals_hpp,
};
DEPS(game) {
	game_hpp,
	globals_hpp, main_menu_hpp, scene_hpp,
};
DEPS(player) { player_hpp, level_hpp };
NO_DEPS(input_manager);
DEPS(actions) { actions_hpp, input_manager_hpp };
DEPS(level) {
	level_hpp,
	actions_hpp, globals_hpp, levels_list_hpp,
	main_menu_hpp, player_hpp, scene_hpp,
};
DEPS(main_menu) {
	main_menu_hpp,
	globals_hpp, level_select_hpp, levels_list_hpp,
};
NO_DEPS(levels_list);
DEPS(gui) { gui_hpp };
DEPS(level_select) {
	level_select_hpp,
	levels_list_hpp, main_menu_hpp,
};

#define STANDARD_FILE(name) \
	{ SRC_DIR #name ".cpp", BUILD_DIR #name ".o", \
	  name ## _deps, ARRAY_LEN(name ## _deps) }

struct Target {
	const char *src;
	const char *obj;
	const char *const *deps;
	size_t n_deps;
} targets[] = {
	STANDARD_FILE(main),
	STANDARD_FILE(game),
	STANDARD_FILE(player),
	STANDARD_FILE(input_manager),
	STANDARD_FILE(actions),
	STANDARD_FILE(level),
	STANDARD_FILE(main_menu),
	STANDARD_FILE(levels_list),
	STANDARD_FILE(gui),
	STANDARD_FILE(level_select),
};

bool check_needs_rebuild(const struct Target *target) {
	if (needs_rebuild1(target->obj, target->src)) return true;
	return needs_rebuild(
		target->obj, (const char**)target->deps, target->n_deps
	);
}

const char *cpp_flags[] = {
	"-g",
	"-Wall", "-Wextra",
	"-I./include", "-I./raylib/src",
};
const char *ld_flags[] = {
	"-O2",
	"-L./raylib/src", "-lraylib", "-lm",
};

void cmd_push(Cmd *cmd, const char **args, size_t args_len) {
	for (size_t i = 0; i < args_len; ++i) {
		cmd_append(cmd, args[i]);
	}
}

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);

	if (!mkdir_if_not_exists(BUILD_DIR)) return 1;

	Cmd cmd = {0};
	Procs procs = {0};

	for (size_t i = 0; i < ARRAY_LEN(targets); ++i) {
		if (!check_needs_rebuild(&targets[i])) continue;

		cmd_append(&cmd, "g++", "-c");
		cmd_push(&cmd, cpp_flags, ARRAY_LEN(cpp_flags));
		cmd_append(&cmd, "-o", targets[i].obj);
		cmd_append(&cmd, targets[i].src);

		if (!cmd_run(&cmd, .async = &procs)) return 1;
	}

	if (!procs_flush(&procs)) return 1;

	bool needs_rebuild = false;

	cmd_append(&cmd, "g++");
	cmd_append(&cmd, "-o", outfile);
	for (size_t i = 0; i < ARRAY_LEN(targets); ++i) {
		cmd_append(&cmd, targets[i].obj);

		if (nob_needs_rebuild1(outfile, targets[i].obj)) needs_rebuild = true;
	}
	for (size_t i = 0; i < ARRAY_LEN(ld_flags); ++i) {
		cmd_append(&cmd, ld_flags[i]);
	}

	if (needs_rebuild) {
		if (!cmd_run(&cmd)) return 1;
	} else nob_log(INFO, "Game executable is up-to-date, not rebuilding!");
}
