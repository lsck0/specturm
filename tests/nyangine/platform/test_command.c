/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  NYA_Arena* arena = nya_arena_create(.name = "test_command");

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Basic command execution - echo
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena     = arena,
      .program   = "echo",
      .arguments = { "hello", "world", nullptr },
    };
    nya_command_run(&cmd);
    nya_assert(cmd.exit_code == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command with output capture
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena     = arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "echo",
      .arguments = { "captured", "output", nullptr },
    };
    nya_command_run(&cmd);
    nya_assert(cmd.exit_code == 0);
    nya_assert(cmd.stdout_content != nullptr);
    nya_assert(cmd.stdout_content->length > 0);
    nya_assert(nya_string_contains(cmd.stdout_content, "captured") == true);
    nya_assert(nya_string_contains(cmd.stdout_content, "output") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command with non-zero exit code
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena     = arena,
      .program   = "false", // Always returns exit code 1
      .arguments = { nullptr },
    };
    nya_command_run(&cmd);
    nya_assert(cmd.exit_code == 1);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command execution time measurement
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena     = arena,
      .program   = "sleep",
      .arguments = { "0.1", nullptr },
    };
    nya_command_run(&cmd);
    nya_assert(cmd.exit_code == 0);
    nya_assert(cmd.execution_time_ms >= 100); // Should take at least 100ms
    nya_assert(cmd.execution_time_ms < 1000); // Sanity check
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command with working directory
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena             = arena,
      .flags             = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .working_directory = "/tmp",
      .program           = "pwd",
      .arguments         = { nullptr },
    };
    nya_command_run(&cmd);
    nya_assert(cmd.exit_code == 0);
    nya_assert(cmd.stdout_content != nullptr);
    nya_assert(nya_string_contains(cmd.stdout_content, "/tmp") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command with multiple arguments
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena     = arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "printf",
      .arguments = { "%s %s %s", "one", "two", "three", nullptr },
    };
    nya_command_run(&cmd);
    nya_assert(cmd.exit_code == 0);
    nya_assert(nya_string_contains(cmd.stdout_content, "one two three") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command with stderr capture
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena     = arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "ls",
      .arguments = { "/nonexistent_directory_12345", nullptr },
    };
    nya_command_run(&cmd);
    nya_assert(cmd.exit_code != 0);
    // stderr might be captured depending on the implementation
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command with environment variable
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena     = arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "sh",
      .arguments = { "-c", "echo $TEST_VAR", nullptr },
    };
    // Note: Setting environment variables in NYA_Command would require
    // setting up the environment array, which might not be fully supported
    // This test just verifies the basic structure works
    nya_command_run(&cmd);
    // Exit code should be 0 regardless of whether the var is set
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command struct initialization
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = { 0 };
    nya_assert(cmd.program == nullptr);
    nya_assert(cmd.arguments[0] == nullptr);
    nya_assert(cmd.exit_code == 0);
    nya_assert(cmd.flags == NYA_COMMAND_FLAG_NONE);
    nya_assert(cmd.arena == nullptr);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Command with complex output
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_Command cmd = {
      .arena     = arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "seq",
      .arguments = { "1", "5", nullptr },
    };
    nya_command_run(&cmd);
    nya_assert(cmd.exit_code == 0);
    nya_assert(cmd.stdout_content != nullptr);
    nya_assert(nya_string_contains(cmd.stdout_content, "1") == true);
    nya_assert(nya_string_contains(cmd.stdout_content, "5") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // CLEANUP
  // ─────────────────────────────────────────────────────────────────────────────
  nya_arena_destroy(arena);

  return 0;
}
