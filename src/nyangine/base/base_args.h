#pragma once

#include "nyangine/base/base_build.h"
#include "nyangine/base/base_string.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define NYA_ARG_MAX_COMMANDS   64
#define NYA_ARG_MAX_PARAMETERS 128

typedef struct NYA_ArgParser    NYA_ArgParser;
typedef struct NYA_ArgCommand   NYA_ArgCommand;
typedef struct NYA_ArgParameter NYA_ArgParameter;

typedef enum {
  NYA_ARG_PARAMETER_KIND_FLAG,
  NYA_ARG_PARAMETER_KIND_POSITIONAL,
} NYA_ArgParameterKind;

struct NYA_ArgParameter {
  NYA_ArgParameterKind kind;
  b8                   variadic;

  NYA_ConstCString name;
  NYA_ConstCString description;
  NYA_Value        default_value;

  /** will be filled after parsing */

  b8 was_matched;

  /** used for single parameters */
  NYA_Value value;

  /** used for variadic parameters */
  u32       values_count;
  NYA_Value values[NYA_ARG_MAX_PARAMETERS];
};

struct NYA_ArgCommand {
  b8 is_root;

  NYA_ConstCString name;
  NYA_ConstCString description;

  NYA_ArgCommand*   subcommands[NYA_ARG_MAX_COMMANDS];
  NYA_ArgParameter* parameters[NYA_ARG_MAX_PARAMETERS];

  /** either handler or build_rule must be set */

  /// bad subcommand, only flags are parsed
  b8 incomplete;
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

NYA_API NYA_EXTERN NYA_ArgCommand* nya_args_parse_argv(NYA_ArgParser* parser, s32 argc, NYA_CString* argv);
NYA_API NYA_EXTERN b8              nya_args_run_command(NYA_ArgCommand* command);
NYA_API NYA_EXTERN void            nya_args_print_usage(NYA_ArgParser* parser, NYA_ArgCommand* command_override);
