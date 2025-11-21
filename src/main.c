#include "nyangine/base/base_perf.h"

#include "gnyame/gnyame.c"
#include "gnyame/gnyame.h"

s32 main(s32 argc, char** argv) {
  nya_unused(argc, argv);

  // gnyame_setup(argc, argv);
  // gnyame_run();
  // gnyame_shutdown();
  // return 0;

  NYA_Arena arena = nya_arena_new();

  NYA_Command cmd = {
      .arena             = &arena,
      .working_directory = ".",
      .program           = "echo",
      .arguments         = {"ur", "mom"},
      .environment       = {"KEY=VALUE"},
  };
  nya_command_run(&cmd);

  nya_info("Command exited with code %d", cmd.exit_code);
  nya_info("Command ran for %lu ms", cmd.execution_time_ms);
  nya_info("Command stdout: " NYA_FMT_STRING, NYA_FMT_STRING_ARG(cmd.stdout_content));
  nya_info("Command stderr: " NYA_FMT_STRING, NYA_FMT_STRING_ARG(cmd.stderr_content));

  nya_arena_destroy(&arena);
}
