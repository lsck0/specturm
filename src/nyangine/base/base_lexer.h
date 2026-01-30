/**
 * @file base_lexer.h
 *
 * Simple lexer/tokenizer.
 *
 * Example:
 * ```c
 * NYA_Lexer nya_lexer_create("let x = 42;");
 * nya_lexer_run(&lexer);
 *
 * nya_array_foreach (lexer.tokens, token) { ... }
 *
 * nya_lexer_destroy(&lexer);
 * ```
 * */
#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_string.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef enum NYA_TokenType NYA_TokenType;
typedef struct NYA_Token   NYA_Token;
typedef struct NYA_Lexer   NYA_Lexer;
nya_derive_array(NYA_Token);

enum NYA_TokenType {
  NYA_TOKEN_INVALID,

  NYA_TOKEN_EOF,
  NYA_TOKEN_SYMBOL,
  NYA_TOKEN_IDENT,
  NYA_TOKEN_NUMBER_INTEGER,
  NYA_TOKEN_NUMBER_FLOAT,

  NYA_TOKEN_COUNT,
};

struct NYA_Token {
  NYA_TokenType type;
  u32           source_location;
  u32           length;
  u32           line_number;
  u32           char_number;

  /** only present if type == NYA_TOKEN_SYMBOL */
  union {
    u8 symbol;
  };
};

struct NYA_Lexer {
  NYA_Arena* arena;

  NYA_ConstCString source;
  u32              cursor;

  u32 current_line_number;
  u32 current_char_number;

  /* will be filled after running */

  NYA_TokenArray* tokens;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN NYA_Lexer nya_lexer_create(NYA_ConstCString source);
NYA_API NYA_EXTERN void      nya_lexer_run(NYA_Lexer* lexer);
NYA_API NYA_EXTERN void      nya_lexer_destroy(NYA_Lexer* lexer);
