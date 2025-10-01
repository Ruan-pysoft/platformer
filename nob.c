// see https://github.com/tsoding/nob.h/blob/main/how_to/001_basic_usage/nob.c

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOB_WARN_DEPRECATED
#include "nob.h"
#include "src_build/dirs.h"

extern const char *initial_config;

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "nob.h", "src_build/dirs.h");

	if (!mkdir_if_not_exists(BUILD_DIR)) return 1;

	Cmd cmd = {0};

	const char *conf_path = BUILD_DIR"config.h";
	int exists = file_exists(conf_path);
	if (exists < 0) return 1;
	if (exists == 0) {
		nob_log(INFO, "Generating initial %s", conf_path);
		if (!write_entire_file(
			conf_path, initial_config, strlen(initial_config)
		)) return 1;
		nob_log(INFO, "==================================");
		nob_log(INFO, "EDIT %s TO CONFIGURE YOUR BUILD!!!", conf_path);
		nob_log(INFO, "==================================");
	}

	const char *input_path = SRC_BUILD_DIR"nob_configed.c";
	const char *output_path = BUILD_DIR"nob_configed";
	nob_cc(&cmd);
	cmd_append(&cmd, "-I.", "-I"BUILD_DIR, "-I"SRC_BUILD_DIR);
	nob_cc_output(&cmd, output_path);
	nob_cc_inputs(&cmd, input_path);
	if (!cmd_run(&cmd)) return 1;

	cmd_append(&cmd, output_path);
	if (!cmd_run(&cmd)) return 1;

	return 0;
}

#define nl "\n"
const char *initial_config =
	"#ifndef CONFIG_H_"nl
	"#define CONFIG_H_"nl
	nl
	"// #define RELEASE // Enable release build"nl
	"// #define WINDOWS // Cross-compile for windows"nl
	nl
	"#endif /* CONFIG_H_ */"nl
;
