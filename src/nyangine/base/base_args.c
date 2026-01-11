#include "nyangine/base/base_args.h"

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

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void nya_args_parse(NYA_ArgParser* parser, s32 argc, NYA_CString* argv) {
  nya_unused(parser);
  nya_unused(argc);
  nya_unused(argv);
}

void nya_args_print_usage(NYA_ArgParser* parser, NYA_ArgCommand* command_override) {
  _nya_args_validate_parser(parser);

  NYA_ArgCommand* command_to_use = command_override ? command_override : parser->root_command;

  // pre-pass
  u8 name_max_length  = 0;
  b8 subcommand_found = false;
  b8 positional_found = false;
  b8 flag_found       = false;
  for (u32 option_index = 0; option_index < NYA_ARG_MAX_OPTIONS; option_index++) {
    NYA_ArgOption* option = command_to_use->options[option_index];
    if (option == nullptr) break;

    if (option->kind == NYA_ARG_OPTION_KIND_POSITIONAL) {
      positional_found = true;
      u8 name_length   = nya_cast_to_u8(strlen(option->name));
      if (name_length > name_max_length) name_max_length = name_length;
    }

    if (option->kind == NYA_ARG_OPTION_KIND_FLAG) {
      flag_found     = true;
      u8 name_length = nya_cast_to_u8(strlen(option->name)) + 2; // +2 to account for the '--' prefix
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

    for (u32 option_index = 0; option_index < NYA_ARG_MAX_OPTIONS; option_index++) {
      NYA_ArgOption* option = command_to_use->options[option_index];
      if (option == nullptr) break;

      if (option->kind == NYA_ARG_OPTION_KIND_POSITIONAL) {
        if (option->variadic) {
          printf("<%s...> ", option->name);
        } else {
          printf("<%s> ", option->name);
        }
      }
    }

    if (flag_found) printf("[options]");

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
    for (u32 option_index = 0; option_index < NYA_ARG_MAX_OPTIONS; option_index++) {
      NYA_ArgOption* option = command_to_use->options[option_index];
      if (option == nullptr) break;
      if (option->kind != NYA_ARG_OPTION_KIND_POSITIONAL) continue;

      printf(
          "  %-*s : %-*s %s\n",
          name_max_length,
          option->name,
          _NYA_ARGS_DESCRIPTION_INDENT,
          NYA_TYPE_NAME_MAP[option->value.type],
          option->description ? option->description : ""
      );
    }
  }

  // print flag details
  if (flag_found) {
    printf("\nOptions:\n");
    for (u32 option_index = 0; option_index < NYA_ARG_MAX_OPTIONS; option_index++) {
      NYA_ArgOption* option = command_to_use->options[option_index];
      if (option == nullptr) break;
      if (option->kind != NYA_ARG_OPTION_KIND_FLAG) continue;

      printf(
          "  --%-*s : %-*s %s",
          name_max_length - 2,
          option->name,
          _NYA_ARGS_DESCRIPTION_INDENT,
          NYA_TYPE_NAME_MAP[option->value.type],
          option->description ? option->description : ""
      );

      if (option->default_value.type != NYA_TYPE_NULL) {
        printf(" (default: ");
        switch (option->default_value.type) {
          case NYA_TYPE_BOOL:    printf("%s", option->default_value.as_bool ? "true" : "false"); break;
          case NYA_TYPE_INTEGER: printf(FMTu64, option->default_value.as_integer); break;
          case NYA_TYPE_FLOAT:   printf(FMTf64, option->default_value.as_float); break;
          case NYA_TYPE_STRING:  printf("%s", option->default_value.as_string); break;
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

  // validate options
  b8 variadic_found = false;
  for (u32 option_index = 0; option_index < NYA_ARG_MAX_OPTIONS; option_index++) {
    NYA_ArgOption* option = command->options[option_index];
    if (option == nullptr) break;

    nya_assert(option->name);
    if (option->value.type != NYA_TYPE_BOOL && option->value.type != NYA_TYPE_INTEGER &&
        option->value.type != NYA_TYPE_FLOAT && option->value.type != NYA_TYPE_STRING) {
      nya_panic(
          "Option '%s' in command '%s' has invalid type. Has to be BOOL, INTEGER, FLOAT or STRING.",
          option->name,
          command->name
      );
    }

    if (option->kind == NYA_ARG_OPTION_KIND_POSITIONAL) {
      nya_assert(command->subcommands[0] == nullptr, "Can either have positional arguments or subcommands, not both.");
      nya_assert(option->default_value.type == NYA_TYPE_NULL, "Positional argument cannot be optional.");
      if (variadic_found) {
        nya_panic(
            "Only the last positional argument in command '%s' can be variadic. Option '%s' is variadic but is not "
            "the "
            "last positional argument.",
            command->name ? command->name : "root",
            option->name
        );
      } else {
        if (option->variadic) variadic_found = true;
      }
    }

    if (option->kind == NYA_ARG_OPTION_KIND_FLAG && option->default_value.type != NYA_TYPE_NULL) {
      if (option->default_value.type != option->value.type) {
        nya_panic(
            "Option '%s' in command '%s' has mismatched default value type. Expected %s but got %s.",
            option->name,
            command->name,
            NYA_TYPE_NAME_MAP[option->value.type],
            NYA_TYPE_NAME_MAP[option->default_value.type]
        );
      }
    }

    // validate subcommands
    for (u32 subcommand_index = 0; subcommand_index < NYA_ARG_MAX_COMMANDS; subcommand_index++) {
      NYA_ArgCommand* subcommand = command->subcommands[subcommand_index];
      if (subcommand == nullptr) break;

      _nya_args_validate_command_tree(subcommand);
    }
  }
}

void _nya_args_print_command_path(NYA_ArgParser* parser, NYA_ArgCommand* command) {
  static NYA_ArgCommand* path[NYA_ARG_MAX_COMMANDS];
  static u32             path_length = 0;

  if (path_length == 0) path[path_length++] = parser->root_command;

  // depth first search
  for (u32 subcommand_index = 0; subcommand_index < NYA_ARG_MAX_COMMANDS; subcommand_index++) {
    NYA_ArgCommand* subcommand = path[path_length - 1]->subcommands[subcommand_index];
    if (subcommand == nullptr) continue;

    path[path_length++] = subcommand;

    if (subcommand == command) {
      for (u32 path_index = 0; path_index < path_length; path_index++) {
        if (path[path_index]->name) printf("%s ", path[path_index]->name);
      }
      return;
    }

    _nya_args_print_command_path(parser, command);
    path_length--;
  }
}
