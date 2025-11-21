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
  NYA_BUILD_ONCE,
  NYA_BUILD_ALWAYS,
  // NYA_BUILD_CACHE,
  NYA_BUILD_COUNT,
} NYA_BuildRulePolicy;

struct NYA_BuildRule {
  NYA_ConstCString    name;
  b8                  is_phony;
  NYA_BuildRulePolicy policy;

  NYA_ConstCString input_file;
  NYA_ConstCString output_file;
  NYA_Command      command;

  NYA_BuildRule* dependencies[NYA_BUILD_MAX_DEPENDENCIES];
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_build(NYA_BuildRule* build_rule);
