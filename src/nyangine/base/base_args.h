#pragma once

#include "nyangine/base/base_build.h"
#include "nyangine/base/base_string.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define NYA_ARG_MAX_COMMANDS 64
#define NYA_ARG_MAX_OPTIONS  64

typedef struct NYA_ArgParser  NYA_ArgParser;
typedef struct NYA_ArgCommand NYA_ArgCommand;
typedef struct NYA_ArgOption  NYA_ArgOption;

typedef enum {
  NYA_ARG_OPTION_KIND_FLAG,
  NYA_ARG_OPTION_KIND_POSITIONAL,
} NYA_ArgOptionKind;

struct NYA_ArgOption {
  NYA_ArgOptionKind kind;
  b8                variadic;

  NYA_ConstCString name;
  NYA_ConstCString description;
  NYA_Value        default_value;

  /** will be filled after parsing */

  NYA_Value value;
};

struct NYA_ArgCommand {
  b8 is_root;

  NYA_ConstCString name;
  NYA_ConstCString description;

  NYA_ArgCommand* subcommands[NYA_ARG_MAX_COMMANDS];
  NYA_ArgOption*  options[NYA_ARG_MAX_OPTIONS];

  void (*handler)(NYA_ArgCommand* command);
  NYA_BuildRule* build_rule;
};

struct NYA_ArgParser {
  NYA_ConstCString name;
  NYA_ConstCString version;
  NYA_ConstCString author;
  NYA_ConstCString description;

  NYA_CString     executable_name;
  NYA_ArgCommand* root_command;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_args_parse(NYA_ArgParser* parser, s32 argc, NYA_CString* argv);
NYA_API NYA_EXTERN void nya_args_print_usage(NYA_ArgParser* parser, NYA_ArgCommand* command_override);
