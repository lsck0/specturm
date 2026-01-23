#include "nyangine/base/base_string.h"
#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define _NYA_ARGS_DESCRIPTION_INDENT 16

NYA_INTERNAL void _nya_args_validate_parser(NYA_ArgParser* parser);
NYA_INTERNAL void _nya_args_validate_command_tree(NYA_ArgCommand* command);
NYA_INTERNAL void _nya_args_print_command_path(NYA_ArgParser* parser, NYA_ArgCommand* command);
NYA_INTERNAL void _nya_args_print_error_and_exit(NYA_ConstCString format, ...) __attr_fmt_printf(1, 2);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_ArgCommand* nya_args_parse_argv(NYA_ArgParser* parser, s32 argc, NYA_CString* argv) {
  _nya_args_validate_parser(parser);

  NYA_ArgCommand* command_to_execute         = nullptr;
  NYA_ArgCommand* path[NYA_ARG_MAX_COMMANDS] = { parser->root_command };
  u32             path_count                 = 1;

  // exec name
  if (parser->executable_name != nullptr) {
    if (!nya_string_equals(argv[0], parser->executable_name)) {
      _nya_args_print_error_and_exit("Executable name mismatch. Expected '%s' but got '%s'.", parser->executable_name, argv[0]);
    }
  }

subcommand_matching:
  for (u32 subcommand_index = 0; subcommand_index < NYA_ARG_MAX_COMMANDS; subcommand_index++) {
    NYA_ArgCommand* subcommand = path[path_count - 1]->subcommands[subcommand_index];
    if (subcommand == nullptr) continue;

    if (argc > 1 && nya_string_equals(argv[1], subcommand->name)) {
      argc--;
      argv++;
      path[path_count++] = subcommand;
      goto subcommand_matching;
    }
  }

  // final command to execute
  command_to_execute      = path[path_count - 1];
  b8 bad_input_flags_only = false;
  if (command_to_execute->subcommands[0] != nullptr) {
    bad_input_flags_only           = true;
    command_to_execute->incomplete = true;
  }

  // if we have a subcommand, remove it from the args
  if (path_count > 1 && !bad_input_flags_only) {
    argc--;
    argv++;
  }

  for (s32 arg_index = 0; arg_index < argc; arg_index++) {
    if (argv[arg_index] == nullptr) continue;

    if (nya_string_starts_with(argv[arg_index], "--")) {
      // we potentially have a flag
      NYA_CString       flag_name = argv[arg_index] + 2; // skip the '--' prefix
      NYA_ArgParameter* param     = nullptr;

      // flags can be of parents too, search the path
      for (u32 path_index = path_count; path_index > 0; path_index--) {
        NYA_ArgCommand* command_in_path = path[path_index - 1];
        for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
          NYA_ArgParameter* candidate = command_in_path->parameters[param_index];
          if (candidate == nullptr) break;
          if (candidate->kind == NYA_ARG_PARAMETER_KIND_FLAG && nya_string_equals(candidate->name, flag_name)) {
            param = candidate;
            goto end_parameter_search;
          }
        }
      }

    end_parameter_search:
      if (param != nullptr) {
        param->was_matched = true;
      } else {
        _nya_args_print_error_and_exit("Unexpected flag: '--%s'.", flag_name);
      }

      // match on type
      switch (param->value.type) {
        case NYA_TYPE_BOOL: {
          // boolean flags can be value-less -> true
          NYA_CString flag_value_str = nullptr;
          if (arg_index + 1 < argc) flag_value_str = argv[arg_index + 1];

          if (flag_value_str && nya_string_equals(flag_value_str, "true")) {
            param->value.as_bool = true;
            arg_index++;
          } else if (flag_value_str && nya_string_equals(flag_value_str, "false")) {
            param->value.as_bool = false;
            arg_index++;
          } else if (!flag_value_str || nya_string_starts_with(flag_value_str, "--")) {
            param->value.as_bool = true;
          } else {
            _nya_args_print_error_and_exit("Failed to parse boolean value for flag '--%s': '%s'.", param->name, flag_value_str);
          }
        } break;

        case NYA_TYPE_INTEGER: {
          NYA_CString flag_value_str = nullptr;
          if (arg_index + 1 < argc) {
            flag_value_str = argv[arg_index + 1];
          } else {
            _nya_args_print_error_and_exit("Flag '--%s' expects a value, but none was provided.", param->name);
          }

          char* end_ptr = nullptr;
          s64   value   = strtol(flag_value_str, &end_ptr, 10);
          if (end_ptr == flag_value_str || *end_ptr != '\0') {
            _nya_args_print_error_and_exit("Failed to parse integer value for flag '--%s': '%s'.", param->name, flag_value_str);
          }
          param->value.as_integer = value;
          arg_index++;
        } break;

        case NYA_TYPE_FLOAT: {
          NYA_CString flag_value_str = nullptr;
          if (arg_index + 1 < argc) {
            flag_value_str = argv[arg_index + 1];
          } else {
            _nya_args_print_error_and_exit("Flag '--%s' expects a value, but none was provided.", param->name);
          }

          char* end_ptr = nullptr;
          f64   value   = strtod(flag_value_str, &end_ptr);
          if (end_ptr == flag_value_str || *end_ptr != '\0') {
            _nya_args_print_error_and_exit("Failed to parse float value for flag '--%s': '%s'.", param->name, flag_value_str);
          }
          param->value.as_float = value;
          arg_index++;
        } break;

        case NYA_TYPE_STRING: {
          NYA_CString flag_value_str = nullptr;
          if (arg_index + 1 < argc) {
            flag_value_str = argv[arg_index + 1];
          } else {
            _nya_args_print_error_and_exit("Flag '--%s' expects a value, but none was provided.", param->name);
          }

          param->value.as_string = flag_value_str;
          arg_index++;
        } break;

        default: nya_unreachable();
      }
    } else {
      // positional argument

      // bad input to begin with, but maybe user wanted help on a subcommand
      if (bad_input_flags_only) continue;

      // find first positional parameter that is not filled yet
      NYA_ArgParameter* param = nullptr;
      for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
        NYA_ArgParameter* candidate = command_to_execute->parameters[param_index];
        if (candidate == nullptr) break;
        if (candidate->kind == NYA_ARG_PARAMETER_KIND_POSITIONAL && !candidate->was_matched) {
          param = candidate;
          break;
        }
      }

      if (param != nullptr) {
        param->was_matched = true;
      } else {
        _nya_args_print_error_and_exit("Unexpected positional argument: '%s'.", argv[arg_index]);
      }

      // handle variadic
      if (param->variadic) {
        // consume all remaining args, upto the next --flag (if any)
        while (arg_index < argc) {
          if (nya_string_starts_with(argv[arg_index], "--")) {
            arg_index--;
            break;
          }

          // match on type
          switch (param->value.type) {
            case NYA_TYPE_BOOL: {
              if (nya_string_equals(argv[arg_index], "true")) {
                param->values[param->values_count].as_bool = true;
              } else if (nya_string_equals(argv[arg_index], "false")) {
                param->values[param->values_count].as_bool = false;
              } else {
                _nya_args_print_error_and_exit("Failed to parse boolean value for argument '%s': '%s'.", param->name, argv[arg_index]);
              }
            } break;

            case NYA_TYPE_INTEGER: {
              char* end_ptr = nullptr;
              s64   value   = strtol(argv[arg_index], &end_ptr, 10);
              if (end_ptr == argv[arg_index] || *end_ptr != '\0') {
                _nya_args_print_error_and_exit("Failed to parse integer value for argument '%s': '%s'.", param->name, argv[arg_index]);
              }
              param->values[param->values_count].as_integer = value;
            } break;

            case NYA_TYPE_FLOAT: {
              char* end_ptr = nullptr;
              f64   value   = strtod(argv[arg_index], &end_ptr);
              if (end_ptr == argv[arg_index] || *end_ptr != '\0') {
                _nya_args_print_error_and_exit("Failed to parse float value for argument '%s': '%s'.", param->name, argv[arg_index]);
              }
              param->values[param->values_count].as_float = value;
            } break;

            case NYA_TYPE_STRING: {
              param->values[param->values_count].as_string = argv[arg_index];
            } break;

            default: nya_unreachable();
          }

          // increment
          param->values_count++;
          arg_index++;
        }
      } else {

        // match on type
        switch (param->value.type) {
          case NYA_TYPE_BOOL: {
            if (nya_string_equals(argv[arg_index], "true")) {
              param->value.as_bool = true;
            } else if (nya_string_equals(argv[arg_index], "false")) {
              param->value.as_bool = false;
            } else {
              _nya_args_print_error_and_exit("Failed to parse boolean value for argument '%s': '%s'.", param->name, argv[arg_index]);
            }
          } break;

          case NYA_TYPE_INTEGER: {
            char* end_ptr = nullptr;
            s64   value   = strtol(argv[arg_index], &end_ptr, 10);
            if (end_ptr == argv[arg_index] || *end_ptr != '\0') {
              _nya_args_print_error_and_exit("Failed to parse integer value for argument '%s': '%s'.", param->name, argv[arg_index]);
            }
            param->value.as_integer = value;
          } break;

          case NYA_TYPE_FLOAT: {
            char* end_ptr = nullptr;
            f64   value   = strtod(argv[arg_index], &end_ptr);
            if (end_ptr == argv[arg_index] || *end_ptr != '\0') {
              _nya_args_print_error_and_exit("Failed to parse float value for argument '%s': '%s'.", param->name, argv[arg_index]);
            }
            param->value.as_float = value;
          } break;

          case NYA_TYPE_STRING: {
            param->value.as_string = argv[arg_index];
          } break;

          default: nya_unreachable();
        }
      }
    }
  }

  // check if --help was passed (skip missing args check if so)
  b8 help_requested = false;
  for (u32 path_index = 0; path_index < path_count; path_index++) {
    NYA_ArgCommand* command_in_path = path[path_index];
    for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
      NYA_ArgParameter* param = command_in_path->parameters[param_index];
      if (param == nullptr) break;
      if (param->kind == NYA_ARG_PARAMETER_KIND_FLAG && nya_string_equals(param->name, "help") && param->was_matched) {
        help_requested = true;
        break;
      }
    }
    if (help_requested) break;
  }

  // recap: report missing and fill defaults
  for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
    NYA_ArgParameter* param = command_to_execute->parameters[param_index];
    if (param == nullptr) break;

    if (!param->was_matched) {
      if (param->default_value.type != NYA_TYPE_NULL) {
        param->value = param->default_value;
      } else if (param->kind == NYA_ARG_PARAMETER_KIND_FLAG && param->value.type == NYA_TYPE_BOOL) {
        // boolean flags default to false
        param->value.as_bool = false;
      } else if (param->kind == NYA_ARG_PARAMETER_KIND_POSITIONAL && param->variadic) {
        // variadic positional arguments are optional by default (values_count stays 0)
      } else if (!help_requested) {
        _nya_args_print_error_and_exit(
            "Missing required parameter '%s' for command '%s'.",
            param->name,
            command_to_execute->name ? command_to_execute->name : "root"
        );
      }
    }
  }

  return command_to_execute;
}

b8 nya_args_run_command(NYA_ArgCommand* command) {
  nya_assert(command);

  if (command->incomplete) {
    printf("Error: No subcommand provided for command '%s'.\n\n", command->name ? command->name : "root");
    return false;
  }

  if (command->handler != nullptr) {
    command->handler(command);
  } else if (command->build_rule != nullptr) {
    nya_build(command->build_rule);
  } else {
    nya_panic("Command '%s' has neither a handler nor a build rule.", command->name ? command->name : "root");
  }

  return true;
}

void nya_args_print_usage(NYA_ArgParser* parser, NYA_ArgCommand* command_override) {
  _nya_args_validate_parser(parser);

  NYA_ArgCommand* command_to_use = command_override ? command_override : parser->root_command;

  // pre-pass
  u8 name_max_length  = 0;
  b8 subcommand_found = false;
  b8 positional_found = false;
  b8 flag_found       = false;
  for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
    NYA_ArgParameter* param = command_to_use->parameters[param_index];
    if (param == nullptr) break;

    if (param->kind == NYA_ARG_PARAMETER_KIND_POSITIONAL) {
      positional_found = true;
      u8 name_length   = nya_cast_to_u8(strlen(param->name));
      if (name_length > name_max_length) name_max_length = name_length;
    }

    if (param->kind == NYA_ARG_PARAMETER_KIND_FLAG) {
      flag_found     = true;
      u8 name_length = nya_cast_to_u8(strlen(param->name)) + 2; // +2 to account for the '--' prefix
      if (name_length > name_max_length) name_max_length = name_length;
    }
  }
  for (u32 subcommand_index = 0; subcommand_index < NYA_ARG_MAX_COMMANDS; subcommand_index++) {
    NYA_ArgCommand* subcommand = command_to_use->subcommands[subcommand_index];
    if (subcommand == nullptr) break;

    subcommand_found = true;
    u8 name_length   = nya_cast_to_u8(strlen(subcommand->name));
    if (name_length > name_max_length) name_max_length = name_length;
  }

  // print header
  if (command_override == nullptr || command_override->is_root) {
    printf("%s\n", parser->name);
    if (parser->version) printf("Version: %s\n", parser->version);
    if (parser->author) printf("Author: %s\n", parser->author);
    if (parser->description) printf("%s\n", parser->description);
  } else {
    printf("%s\n", command_override->description);
  }

  printf("\n");

  // assemble usage line
  if (subcommand_found) {
    printf("Usage: %s [command]\n", parser->executable_name);
  } else {
    printf("Usage: %s ", parser->executable_name);
    if (command_override) _nya_args_print_command_path(parser, command_override);

    for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
      NYA_ArgParameter* param = command_to_use->parameters[param_index];
      if (param == nullptr) break;

      if (param->kind == NYA_ARG_PARAMETER_KIND_POSITIONAL) {
        if (param->variadic) {
          printf("<%s...> ", param->name);
        } else {
          printf("<%s> ", param->name);
        }
      }
    }

    if (flag_found) printf("[flags]");

    printf("\n");
  }

  // print subcommand details
  if (subcommand_found) {
    printf("\nCommands:\n");
    for (u32 subcommand_index = 0; subcommand_index < NYA_ARG_MAX_COMMANDS; subcommand_index++) {
      NYA_ArgCommand* subcommand = command_to_use->subcommands[subcommand_index];
      if (subcommand == nullptr) break;

      printf(
          "  %-*s   %-*s %s\n",
          name_max_length,
          subcommand->name,
          _NYA_ARGS_DESCRIPTION_INDENT,
          "", // no type for subcommands
          subcommand->description ? subcommand->description : ""
      );
    }
  }

  // print argument details
  if (positional_found) {
    printf("\nArguments:\n");
    for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
      NYA_ArgParameter* param = command_to_use->parameters[param_index];
      if (param == nullptr) break;
      if (param->kind != NYA_ARG_PARAMETER_KIND_POSITIONAL) continue;

      printf(
          "  %-*s : %-*s %s\n",
          name_max_length,
          param->name,
          _NYA_ARGS_DESCRIPTION_INDENT,
          NYA_TYPE_NAME_MAP[param->value.type],
          param->description ? param->description : ""
      );
    }
  }

  // print flag details
  if (flag_found) {
    printf("\nFlags:\n");
    for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
      NYA_ArgParameter* param = command_to_use->parameters[param_index];
      if (param == nullptr) break;
      if (param->kind != NYA_ARG_PARAMETER_KIND_FLAG) continue;

      printf(
          "  --%-*s : %-*s %s",
          name_max_length - 2,
          param->name,
          _NYA_ARGS_DESCRIPTION_INDENT,
          NYA_TYPE_NAME_MAP[param->value.type],
          param->description ? param->description : ""
      );

      if (param->default_value.type != NYA_TYPE_NULL) {
        printf(" (default: ");
        switch (param->default_value.type) {
          case NYA_TYPE_BOOL:    printf("%s", param->default_value.as_bool ? "true" : "false"); break;
          case NYA_TYPE_INTEGER: printf(FMTu64, param->default_value.as_integer); break;
          case NYA_TYPE_FLOAT:   printf(FMTf64, param->default_value.as_float); break;
          case NYA_TYPE_STRING:  printf("%s", param->default_value.as_string); break;
          default:               printf("unknown"); break;
        }
        printf(")");
      }

      printf("\n");
    }
  }
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void _nya_args_validate_parser(NYA_ArgParser* parser) {
  nya_assert(parser);

  nya_assert(parser->name);
  _nya_args_validate_command_tree(parser->root_command);
}

void _nya_args_validate_command_tree(NYA_ArgCommand* command) {
  nya_assert(command);

  // validate command properties
  if (command->is_root) {
    nya_assert(command->name == nullptr, "Root command should not have a name.");
    nya_assert(command->description == nullptr, "Root command should not have a description.");
  } else {
    nya_assert(command->name, "Non-root command must have a name.");
  }

  // validate parameters
  b8 has_positionals = false;
  b8 variadic_found  = false;
  for (u32 param_index = 0; param_index < NYA_ARG_MAX_PARAMETERS; param_index++) {
    NYA_ArgParameter* param = command->parameters[param_index];
    if (param == nullptr) break;

    nya_assert(param->name);
    if (param->value.type != NYA_TYPE_BOOL && param->value.type != NYA_TYPE_INTEGER && param->value.type != NYA_TYPE_FLOAT &&
        param->value.type != NYA_TYPE_STRING) {
      nya_panic("Parameter '%s' in command '%s' has invalid type. Has to be BOOL, INTEGER, FLOAT or STRING.", param->name, command->name);
    }

    if (param->kind == NYA_ARG_PARAMETER_KIND_POSITIONAL) {
      nya_assert(command->subcommands[0] == nullptr, "Can either have positional arguments or subcommands, not both.");
      nya_assert(param->variadic || param->default_value.type == NYA_TYPE_NULL, "Only variadic positional arguments can be optional.");

      has_positionals = true;

      if (variadic_found) {
        nya_panic(
            "Only the last positional argument in command '%s' can be variadic. "
            "Parameter '%s' is variadic but is not the last positional argument.",
            command->name ? command->name : "root",
            param->name
        );
      } else {
        if (param->variadic) variadic_found = true;
      }
    }

    if (param->kind == NYA_ARG_PARAMETER_KIND_FLAG) {
      nya_assert(!param->variadic, "Flag option cannot be variadic.");

      if (param->default_value.type != NYA_TYPE_NULL && param->default_value.type != param->value.type) {
        nya_panic(
            "Parameter '%s' in command '%s' has mismatched default value type. Expected %s but got %s.",
            param->name,
            command->name,
            NYA_TYPE_NAME_MAP[param->value.type],
            NYA_TYPE_NAME_MAP[param->default_value.type]
        );
      }
    }

    // validate subcommands
    for (u32 subcommand_index = 0; subcommand_index < NYA_ARG_MAX_COMMANDS; subcommand_index++) {
      NYA_ArgCommand* subcommand = command->subcommands[subcommand_index];
      if (subcommand == nullptr) break;

      if (has_positionals) {
        nya_panic("Command '%s' cannot have both positional parameters and subcommands.", command->name ? command->name : "root");
      }

      _nya_args_validate_command_tree(subcommand);
    }
  }

  if (has_positionals) {
    nya_assert(
        command->handler != nullptr || command->build_rule != nullptr,
        "Command '%s' must have either a handler or a build rule associated to it.",
        command->name ? command->name : "root"
    );
  }
}

void _nya_args_print_command_path(NYA_ArgParser* parser, NYA_ArgCommand* command) {
  static NYA_ArgCommand* path[NYA_ARG_MAX_COMMANDS];
  static u32             path_length = 0;
  static b8              found       = false;

  if (path_length == 0) path[path_length++] = parser->root_command;

  // depth first search
  for (u32 subcommand_index = 0; subcommand_index < NYA_ARG_MAX_COMMANDS; subcommand_index++) {
    NYA_ArgCommand* subcommand = path[path_length - 1]->subcommands[subcommand_index];
    if (subcommand == nullptr) continue;

    path[path_length++] = subcommand;

    if (subcommand == command) {
      found = true;
      for (u32 path_index = 0; path_index < path_length; path_index++) {
        if (path[path_index]->name) printf("%s ", path[path_index]->name);
      }
      return;
    }

    _nya_args_print_command_path(parser, command);
    if (found) return;
    path_length--;
  }

  if (path_length == 1) {
    if (!found) nya_panic("Command '%s' not found in parser '%s'.", command->name, parser->name);

    // reset for next use
    path_length = 0;
    found       = false;
  }
}

void _nya_args_print_error_and_exit(NYA_ConstCString format, ...) {
  va_list args;
  va_start(args, format);
  printf("Error: ");
  vprintf(format, args);
  printf("\n\n");
  va_end(args);

  exit(EXIT_FAILURE);
}
