#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define _NYA_BUILD_MAX_BUILD_DEPTH 64

NYA_INTERNAL b8 _nya_build_always(NYA_BuildRule* build_rule);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

b8 nya_build(NYA_BuildRule* build_rule) {
  nya_assert(build_rule);

  switch (build_rule->policy) {
    case NYA_BUILD_ALWAYS: return _nya_build_always(build_rule);

    case NYA_BUILD_ONCE:   {
      nya_assert(build_rule->output_file, "NYA_BUILD_ONCE rules must specify an output_file.");
      if (nya_filesystem_exists(build_rule->output_file)) return true;
      return _nya_build_always(build_rule);
    }

    case NYA_BUILD_IF_OUTDATED: {
      nya_assert(build_rule->input_file, "NYA_BUILD_IF_OUTDATED rules must specify an input_file.");
      nya_assert(build_rule->output_file, "NYA_BUILD_IF_OUTDATED rules must specify an output_file.");

      if (!nya_filesystem_exists(build_rule->output_file)) return _nya_build_always(build_rule);

      u64 input_mod_time  = 0;
      u64 output_mod_time = 0;
      nya_filesystem_last_modified(build_rule->input_file, &input_mod_time);
      nya_filesystem_last_modified(build_rule->output_file, &output_mod_time);

      if (input_mod_time > output_mod_time) return _nya_build_always(build_rule);

      return true;
    }

    default: nya_unreachable();
  }
  static_assert(NYA_BUILD_COUNT == 3, "Unhandled NYA_BuildRulePolicy enum value.");

  nya_unreachable();
}

void nya_rebuild_yourself(s32* argc, NYA_CString* argv, NYA_Command cmd) {
  NYA_CString marker = "--no-rebuild"; // appended to argv

  // check if we've already rebuilt ourselves
  if (nya_string_equals(argv[*argc - 1], marker)) {
    *argc -= 1;
    return;
  }

  NYA_BuildRule rule = {
      .name    = "Rebuild Build System",
      .policy  = NYA_BUILD_ALWAYS,
      .command = cmd,
  };

  // backup, build, restore
  b8 ok;

  // need to not only copy backup, but also permissions and i cant be bothered
  NYA_Command copy_command = {
      .program   = "cp",
      .arguments = {argv[0], ".backup_build_executable"},
  };
  nya_command_run(&copy_command);
  nya_assert(copy_command.exit_code == 0, "Failed to backup build executable before rebuild.");

  ok = nya_build(&rule);
  if (!ok) {
    ok = nya_filesystem_move(".backup_build_executable", argv[0]);
    nya_assert(ok, "Failed to restore backup build executable after failed rebuild.");
    exit(1);
  }

  ok = nya_filesystem_delete(".backup_build_executable");
  nya_assert(ok, "Failed to delete backup build executable after successful rebuild.");

  // build new argv with marker
  NYA_CString* new_argv = nya_alloca((*argc + 2) * sizeof(NYA_CString));
  nya_memcpy(new_argv, argv, (*argc) * sizeof(NYA_CString));
  new_argv[*argc]      = marker;
  new_argv[*argc + 1]  = nullptr;
  *argc               += 1;

  // replace process with the new binary
  execvp(argv[0], (char* const*)new_argv);
  perror("execvp");
  exit(1);
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

b8 _nya_build_always(NYA_BuildRule* build_rule) {
  static u64 depth = 0;

  nya_assert(build_rule);

  // build dependencies first
  for (u64 i = 0; i < NYA_BUILD_MAX_DEPENDENCIES; i++) {
    nya_assert(depth <= _NYA_BUILD_MAX_BUILD_DEPTH, "Maximum build depth exceeded (possible circular dependency).");

    NYA_BuildRule* dependency = build_rule->dependencies[i];
    if (!dependency) break;

    depth++;
    b8 result = nya_build(dependency);
    depth--;

    if (!result) return false;
  }

  // pre-build hooks
  for (u64 i = 0; i < NYA_BUILD_MAX_DEPENDENCIES; i++) {
    void (*hook)(NYA_BuildRule* rule) = build_rule->pre_build_hooks[i];
    if (!hook) break;
    hook(build_rule);
  }

  // running the command
  b8 ok = true;
  if (build_rule->is_metarule) goto skip_build;

  printf("[BUILDING] %s \n", build_rule->name);
  printf("[CMD] %s ", build_rule->command.program);
  for (u64 i = 0; i < NYA_COMMAND_MAX_ARGUMENTS; i++) {
    NYA_ConstCString arg = build_rule->command.arguments[i];
    if (!arg) break;
    printf("%s ", arg);
  }
  printf("\n");

  nya_command_run(&build_rule->command);
  ok = build_rule->command.exit_code == 0;
  if (ok) {
    printf("[OK] Took " FMTu64 " ms.\n\n", build_rule->command.execution_time_ms);
  } else {
    printf("[FAILED] Exit code: %d\n", build_rule->command.exit_code);
    printf("------- STDOUT -------\n" NYA_FMT_STRING "\n", NYA_FMT_STRING_ARG(build_rule->command.stdout_content));
    printf("------- STDERR -------\n" NYA_FMT_STRING "\n", NYA_FMT_STRING_ARG(build_rule->command.stderr_content));
  }

skip_build:

  // post-build hooks
  for (u64 i = 0; i < NYA_BUILD_MAX_DEPENDENCIES; i++) {
    void (*hook)(NYA_BuildRule* rule) = build_rule->post_build_hooks[i];
    if (!hook) break;
    hook(build_rule);
  }

  return ok;
}
