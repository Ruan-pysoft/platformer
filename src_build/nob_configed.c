// see https://github.com/tsoding/nob.h/blob/main/how_to/001_basic_usage/nob.c

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOB_WARN_DEPRECATED
#include "nob.h"

bool build_raylib(void);
bool build_game(void);
bool run_game(void);

int main(int argc, char **argv) {
	if (!build_raylib()) return 1;
	if (!build_game()) return 1;

	bool run = false;
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "run") == 0) run = true;
	}

	if (run && !run_game()) return 1;
}

#include "dirs.h"
#include "config.h"

// add the config.h header as a dependency to the executables,
// as changing the config can affect the compiler used as well as the flags
const char config_header[] = BUILD_DIR"config.h";

/* BUILDING THE RAYLIB LIBRARY */

bool build_raylib(void) {
	fprintf(stderr, "Trying to rebuild raylib...\n");
#ifdef WINDOWS
	nob_log(WARNING, "Cross-building for windows, raylib isn't built locally. Instead download the %s raylib release from https://github.com/raysan5/raylib", "raylib-5.5_win64_mingw-w64");
#else
	const char *libraylib = "raylib/src/libraylib.a";

	// debug raylib and release raylib are different
	bool raylib_needs_rebuild = false;
	if (!raylib_needs_rebuild) {
		nob_log(INFO, "checking if libraylib.a exists...");
		int exists = file_exists(libraylib);
		if (exists < 0) return false;
		raylib_needs_rebuild = exists == 0;
	}
	if (!raylib_needs_rebuild) {
		nob_log(INFO, "checking if %s is newer than %s...", config_header, libraylib);
		raylib_needs_rebuild = needs_rebuild1(libraylib, config_header);
	}
	nob_log(INFO, "Does raylib need rebuild? %s", raylib_needs_rebuild ? "yes" : "no");
	if (!raylib_needs_rebuild) return true;

	if (chdir("raylib/src") == -1) {
		nob_log(ERROR, "Failed cd'ing into raylib/src");
		return false;
	}

	Cmd cmd = {0};

	cmd_append(&cmd, "make", "clean");
	if (!cmd_run(&cmd)) return false;

	cmd_append(&cmd, "make", "PLATFORM=PLATFORM_DESKTOP");
#ifdef WAYLAND
	cmd_append(&cmd, "GLFW_LINUX_ENABLE_WAYLAND=TRUE");
#endif
#ifndef RELEASE
	cmd_append(&cmd, "RAYLIB_BUILD_MODE=DEBUG");
#endif
	cmd_append(&cmd, "-j", temp_sprintf("%d", nprocs()));
	if (!cmd_run(&cmd)) return false;

	if (chdir("../..") == -1) {
		nob_log(ERROR, "Failed cd'ing out of raylib/src");
		return false;
	}
#endif

	return true;
}

/* BUILDING THE GAME EXECUTABLE */

// final executable differs between windows cross-build and native build
#ifdef WINDOWS
const char outfile[] = BUILD_DIR"game.exe";
#else
const char outfile[] = BUILD_DIR"game";
#endif

// list headers used in the project
#define HPP(header) const char *const header ## _hpp = INCLUDE_DIR #header ".hpp"
HPP(actions);
HPP(config);
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

// list the headers each .cpp file depends on
#define HEADERS(of, ...) const char *const of ## _headers[] = { of ## _hpp, __VA_ARGS__ }
#define HEADERS_NO_SELF(of, ...) const char *const of ## _headers[] = { __VA_ARGS__ }

HEADERS_NO_SELF(main, actions_hpp, input_manager_hpp, game_hpp, globals_hpp);
HEADERS(game, globals_hpp, main_menu_hpp, scene_hpp);
HEADERS(player, level_hpp);
HEADERS(input_manager);
HEADERS(actions, input_manager_hpp);
HEADERS(level,
	actions_hpp, globals_hpp, levels_list_hpp, main_menu_hpp, player_hpp,
	scene_hpp
);
HEADERS(main_menu, globals_hpp, level_select_hpp, levels_list_hpp);
HEADERS(levels_list);
HEADERS(gui);
HEADERS(level_select, levels_list_hpp, main_menu_hpp);
HEADERS(config);

// most files in the project exists in src/name.cpp, outputs to build/name.o,
// and depends on the headers defined in name_headers
#define STANDARD_FILE(name) \
	{ SRC_DIR #name ".cpp", BUILD_DIR #name ".o", \
	  name ## _headers, ARRAY_LEN(name ## _headers) }

// list the files in the project
struct Target {
	const char *src;
	const char *obj;
	const char *const *headers;
	size_t n_headers;
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
	STANDARD_FILE(config),
};

// check if a particular file needs rebuilding
bool target_needs_rebuild(const struct Target *target) {
	// if the config header has changed, it needs to be rebuilt
	if (needs_rebuild1(target->obj, config_header)) return true;
	// if the file itself has changed, it needs to be rebuilt
	if (needs_rebuild1(target->obj, target->src)) return true;
	// if any of its headers have changed, it needs to be rebuilt
	return needs_rebuild(
		target->obj, (const char**)target->headers, target->n_headers
	);
}

// select a cross-compiler if required in config.h
#ifdef WINDOWS
const char *compiler = "x86_64-w64-mingw32-g++";
#else
const char *compiler = "g++";
#endif

// flags used for compiling a certain c++ source to a .o file
const char *cpp_flags[] = {
#ifdef RELEASE
	"-O2",
#else
	"-g",
#endif
	"-Wall", "-Wextra",
	"-I./include", "-I./raylib/src",
};
// flags used for compiling all the .o files into the final executable
const char *ld_flags[] = {
	"-O2",
#ifdef WINDOWS
	"-L./raylib-5.5_win64_mingw-w64/lib",
	// idk why, but we need to link extra stuff for it to work on Windows
	// (at least if you plan on running the executable through wine)
	"-lraylib", "-lm", "-lgdi32", "-lwinmm",
	// link it statically, easier than making sure that the end-user has
	// all the libc and libc++ dlls
	// NOTE: this might complicate distribution, as I believe they have
	// the GNU GPL license? So I can't legally share the compiled windows
	// .exe around without the license text and some way for the user to
	// access the source code of the statically linked libraries.
	// A url to whichever git repository my cross-compiler comes from
	// should be enough?
	"-static",
#else
	"-L./raylib/src",
	"-lraylib", "-lm",
#endif
};

// utility to add an array of strings to a cmd; I believe one exists for adding
// a cmd to another one in nob.h, but for some reason not this one? I might
// just be missing it
void cmd_push(Cmd *cmd, const char **args, size_t args_len) {
	for (size_t i = 0; i < args_len; ++i) {
		cmd_append(cmd, args[i]);
	}
}

bool build_game(void) {
	Cmd cmd = {0};
	Procs procs = {0};

	// keep track of if the executable actually needs to be rebuilt; if
	// config.h has changed then it does...
	bool exe_needs_rebuild = needs_rebuild1(outfile, config_header);

	for (size_t i = 0; i < ARRAY_LEN(targets); ++i) {
		// don't rebuild it if it doesn't need to be rebuilt
		// (this saves a *lot* of dev time)
		if (!target_needs_rebuild(&targets[i])) continue;

		// ...and if any .o file was rebuilt, the executable also needs
		// to be rebuilt
		exe_needs_rebuild = true;

		cmd_append(&cmd, compiler, "-c");
		cmd_push(&cmd, cpp_flags, ARRAY_LEN(cpp_flags));
		cmd_append(&cmd, "-o", targets[i].obj);
		cmd_append(&cmd, targets[i].src);

		// and build asynchronously, by default the number of threads
		// is the number of processors on the machine (which might be
		// threads or might be cores, depending on OS, iirc)
		// once again, this saves a *lot* of dev time
		// (I think my laptop has four "processors", which would
		// roughly speed up build times 4x)
		if (!cmd_run(&cmd, .async = &procs)) return false;
	}

	// wait for all .o files to build before linking them together
	if (!procs_flush(&procs)) return false;

	// if neither config.h has changed, nor any .o files were rebuilt, then
	// we don't need to rebuild the executable
	if (exe_needs_rebuild) {
		cmd_append(&cmd, compiler);
		cmd_append(&cmd, "-o", outfile);
		for (size_t i = 0; i < ARRAY_LEN(targets); ++i) {
			cmd_append(&cmd, targets[i].obj);
		}
		// note that (iirc) linker flags like "-lm" should go *after*
		// all the objects, for some reason?
		cmd_push(&cmd, ld_flags, ARRAY_LEN(ld_flags));

		if (!cmd_run(&cmd)) return false;

#ifdef RELEASE
		// and strip the exe on release builds, results in a smaller
		// exe (and theoretically harder to reverse-engineer, but I
		// don't really care about that, I plan on having this
		// open-source anyways)
		cmd_append(&cmd, "strip", outfile);
		if (!cmd_run(&cmd)) return false;
#endif
	} else nob_log(INFO, "Game executable is up-to-date, not rebuilding!");

	return true;
}

/* RUNNING THE COMPILED GAME EXECUTABLE */

bool run_game(void) {
	Cmd cmd = {0};

	cmd_append(&cmd, outfile);
	if (!cmd_run(&cmd)) return false;

	return true;
}
