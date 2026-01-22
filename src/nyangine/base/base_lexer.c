#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_Lexer nya_lexer_create(NYA_ConstCString source) {
  nya_assert(source != nullptr);

  NYA_Lexer lexer = {
    .arena               = nya_arena_create(),
    .source              = source,
    .cursor              = 0,
    .current_line_number = 1,
    .current_char_number = 1,
  };
  lexer.tokens = nya_array_create(&lexer.arena, NYA_Token);

  return lexer;
}

void nya_lexer_run(NYA_Lexer* lexer) {
  nya_assert(lexer != nullptr);

  u8 current_char = lexer->source[lexer->cursor];

  // end of source
  if (current_char == '\0') return;

  // handle whitespace
  if (current_char == ' ' || current_char == '\t' || current_char == '\r') {
    lexer->cursor              += 1;
    lexer->current_char_number += 1;
    goto continue_lexing;
  }
  if (current_char == '\n') {
    lexer->cursor              += 1;
    lexer->current_line_number += 1;
    lexer->current_char_number  = 1;
    goto continue_lexing;
  }

  // lex identifier
  if ((current_char >= 'a' && current_char <= 'z') || (current_char >= 'A' && current_char <= 'Z') || (current_char == '_')) {
    u32 start_cursor      = lexer->cursor;
    u32 start_char_number = lexer->current_char_number;
    u32 start_line_number = lexer->current_line_number;
    while (true) {
      current_char = lexer->source[lexer->cursor];
      if (!((current_char >= 'a' && current_char <= 'z') || (current_char >= 'A' && current_char <= 'Z') ||
            (current_char >= '0' && current_char <= '9') || (current_char == '_'))) {
        break;
      }
      lexer->cursor              += 1;
      lexer->current_char_number += 1;
    }

    NYA_Token token = {
      .type            = NYA_TOKEN_IDENT,
      .source_location = start_cursor,
      .length          = lexer->cursor - start_cursor,
      .line_number     = start_line_number,
      .char_number     = start_char_number,
    };
    nya_array_push_back(&lexer->tokens, token);
    goto continue_lexing;
  }

  // lex number
  if ((current_char >= '0' && current_char <= '9')) {
    u32 start_cursor      = lexer->cursor;
    u32 start_char_number = lexer->current_char_number;
    u32 start_line_number = lexer->current_line_number;
    b8  is_float          = false;

    while (true) {
      current_char = lexer->source[lexer->cursor];
      if (current_char == '.') {
        if (is_float) break;
        is_float                    = true;
        lexer->cursor              += 1;
        lexer->current_char_number += 1;
        continue;
      }
      if (!(current_char >= '0' && current_char <= '9')) break;
      lexer->cursor              += 1;
      lexer->current_char_number += 1;
    }

    // check if floats have at least one digit after the dot
    if (is_float) {
      u32 dot_index = start_cursor;
      while (dot_index < lexer->cursor && lexer->source[dot_index] != '.') dot_index += 1;
      if (dot_index + 1 >= lexer->cursor || !(lexer->source[dot_index + 1] >= '0' && lexer->source[dot_index + 1] <= '9')) {
        lexer->cursor = dot_index;
        is_float      = false;
      }
    }

    NYA_Token token = {
      .type            = is_float ? NYA_TOKEN_NUMBER_FLOAT : NYA_TOKEN_NUMBER_INTEGER,
      .source_location = start_cursor,
      .length          = lexer->cursor - start_cursor,
      .line_number     = start_line_number,
      .char_number     = start_char_number,
    };
    nya_array_push_back(&lexer->tokens, token);
    goto continue_lexing;
  }

  // lex symbol
  if (isprint(current_char)) {
    NYA_Token token = {
      .type            = NYA_TOKEN_SYMBOL,
      .source_location = lexer->cursor,
      .length          = 1,
      .line_number     = lexer->current_line_number,
      .char_number     = lexer->current_char_number,
      .symbol          = current_char,
    };
    nya_array_push_back(&lexer->tokens, token);
    lexer->cursor              += 1;
    lexer->current_char_number += 1;
    goto continue_lexing;
  }

  // invalid character
  NYA_Token token = {
    .type            = NYA_TOKEN_INVALID,
    .source_location = lexer->cursor,
    .length          = 1,
    .line_number     = lexer->current_line_number,
    .char_number     = lexer->current_char_number,
  };
  nya_array_push_back(&lexer->tokens, token);
  lexer->cursor              += 1;
  lexer->current_char_number += 1;

continue_lexing:
  nya_lexer_run(lexer);
}

void nya_lexer_destroy(NYA_Lexer* lexer) {
  nya_assert(lexer != nullptr);

  nya_array_destroy(&lexer->tokens);
  nya_arena_destroy(&lexer->arena);
}
