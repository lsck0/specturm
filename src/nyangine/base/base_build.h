#pragma once

#include "nyangine/base/base_string.h"
#include "nyangine/platform/command/command.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define NYA_BUILD_MAX_DEPENDENCIES 64

typedef struct NYA_BuildRule NYA_BuildRule;

typedef enum {
  NYA_BUILD_ALWAYS,
  NYA_BUILD_ONCE,
  NYA_BUILD_IF_OUTDATED,
  NYA_BUILD_COUNT,
} NYA_BuildRulePolicy;

struct NYA_BuildRule {
  NYA_ConstCString    name;
  NYA_BuildRulePolicy policy;
  b8                  is_metarule; /** metarules dont have commands, only dependencies and hook. */

  NYA_ConstCString input_file;
  NYA_ConstCString output_file;
  NYA_Command      command;

  NYA_BuildRule* dependencies[NYA_BUILD_MAX_DEPENDENCIES];

  void (*pre_build_hooks[NYA_BUILD_MAX_DEPENDENCIES])(NYA_BuildRule* rule);
  void (*post_build_hooks[NYA_BUILD_MAX_DEPENDENCIES])(NYA_BuildRule* rule);
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN b8   nya_build(NYA_BuildRule* build_rule);
NYA_API NYA_EXTERN void nya_rebuild_yourself(s32* argc, NYA_CString* argv, NYA_Command cmd);
