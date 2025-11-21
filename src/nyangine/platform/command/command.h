#pragma once

#include "nyangine/base/base_string.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define NYA_COMMAND_MAX_ARGUMENTS 64
#define NYA_COMMAND_MAX_ENV_VARS  64

typedef struct NYA_Command NYA_Command;

struct NYA_Command {
  NYA_Arena* arena;

  NYA_ConstCString working_directory;
  NYA_ConstCString program;
  NYA_ConstCString arguments[NYA_COMMAND_MAX_ARGUMENTS];
  NYA_CString      environment[NYA_COMMAND_MAX_ENV_VARS];

  /** will be filled after execution */
  s32        exit_code;
  NYA_String stdout_content;
  NYA_String stderr_content;
  u64        execution_time_ms;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_command_run(NYA_Command* command);
NYA_API NYA_EXTERN void nya_command_destroy(NYA_Command* command);
