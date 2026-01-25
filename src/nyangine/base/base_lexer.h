#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_string.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_Token NYA_Token;
typedef struct NYA_Lexer NYA_Lexer;
nya_derive_array(NYA_Token);

typedef enum {
  NYA_TOKEN_INVALID,

  NYA_TOKEN_EOF,
  NYA_TOKEN_SYMBOL,
  NYA_TOKEN_IDENT,
  NYA_TOKEN_NUMBER_INTEGER,
  NYA_TOKEN_NUMBER_FLOAT,

  NYA_TOKEN_COUNT,
} NYA_TokenType;

struct NYA_Token {
  NYA_TokenType type;
  u32           source_location;
  u32           length;
  u32           line_number;
  u32           char_number;

  union {
    u8 symbol;
  };
};

struct NYA_Lexer {
  NYA_Arena arena;

  NYA_ConstCString source;
  u32              cursor;

  u32 current_line_number;
  u32 current_char_number;

  /** will be filled after running */
  NYA_TokenArray tokens;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN NYA_Lexer nya_lexer_create(NYA_ConstCString source);
NYA_API NYA_EXTERN void      nya_lexer_run(NYA_Lexer* lexer);
NYA_API NYA_EXTERN void      nya_lexer_destroy(NYA_Lexer* lexer);
