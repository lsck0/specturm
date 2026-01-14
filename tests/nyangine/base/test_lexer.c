#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  NYA_Lexer lexer = nya_lexer_new("a+b\nc \"2134.34 = .234. asdf! __asd__a __ sad__");
  nya_lexer_run(&lexer);

  NYA_Token token0  = lexer.tokens.items[0];
  NYA_Token token1  = lexer.tokens.items[1];
  NYA_Token token2  = lexer.tokens.items[2];
  NYA_Token token3  = lexer.tokens.items[3];
  NYA_Token token4  = lexer.tokens.items[4];
  NYA_Token token5  = lexer.tokens.items[5];
  NYA_Token token6  = lexer.tokens.items[6];
  NYA_Token token7  = lexer.tokens.items[7];
  NYA_Token token8  = lexer.tokens.items[8];
  NYA_Token token9  = lexer.tokens.items[9];
  NYA_Token token10 = lexer.tokens.items[10];
  NYA_Token token11 = lexer.tokens.items[11];
  NYA_Token token12 = lexer.tokens.items[12];
  NYA_Token token13 = lexer.tokens.items[13];
  NYA_Token token14 = lexer.tokens.items[14];
  nya_assert_panic(nya_array_get(&lexer.tokens, 15));

  nya_assert(token0.type == NYA_TOKEN_IDENT);
  nya_assert(token0.length == 1);
  nya_assert(nya_memcmp(lexer.source + token0.source_location, "a", 1) == 0);

  nya_assert(token1.type == NYA_TOKEN_SYMBOL);
  nya_assert(token1.symbol == '+');

  nya_assert(token2.type == NYA_TOKEN_IDENT);
  nya_assert(token2.length == 1);
  nya_assert(nya_memcmp(lexer.source + token2.source_location, "b", 1) == 0);

  nya_assert(token3.type == NYA_TOKEN_IDENT);
  nya_assert(token3.length == 1);
  nya_assert(nya_memcmp(lexer.source + token3.source_location, "c", 1) == 0);

  nya_assert(token4.type == NYA_TOKEN_SYMBOL);
  nya_assert(token4.symbol == '"');

  nya_assert(token5.type == NYA_TOKEN_NUMBER_FLOAT);
  nya_assert(token5.length == 7);
  nya_assert(nya_memcmp(lexer.source + token5.source_location, "2134.34", 7) == 0);

  nya_assert(token6.type == NYA_TOKEN_SYMBOL);
  nya_assert(token6.symbol == '=');

  nya_assert(token7.type == NYA_TOKEN_SYMBOL);
  nya_assert(token7.symbol == '.');

  nya_assert(token8.type == NYA_TOKEN_NUMBER_INTEGER);
  nya_assert(token8.length == 3);
  nya_assert(nya_memcmp(lexer.source + token8.source_location, "234", 3) == 0);

  nya_assert(token9.type == NYA_TOKEN_SYMBOL);
  nya_assert(token9.symbol == '.');

  nya_assert(token10.type == NYA_TOKEN_IDENT);
  nya_assert(token10.length == 4);
  nya_assert(nya_memcmp(lexer.source + token10.source_location, "asdf", 4) == 0);

  nya_assert(token11.type == NYA_TOKEN_SYMBOL);
  nya_assert(token11.symbol == '!');

  nya_assert(token12.type == NYA_TOKEN_IDENT);
  nya_assert(token12.length == 8);
  nya_assert(nya_memcmp(lexer.source + token12.source_location, "__asd__a", 8) == 0);

  nya_assert(token13.type == NYA_TOKEN_IDENT);
  nya_assert(token13.length == 2);
  nya_assert(nya_memcmp(lexer.source + token13.source_location, "__", 2) == 0);

  nya_assert(token14.type == NYA_TOKEN_IDENT);
  nya_assert(token14.length == 5);
  nya_assert(nya_memcmp(lexer.source + token14.source_location, "sad__", 5) == 0);

  // nya_array_foreach (&lexer.tokens, token) {
  //   switch (token->type) {
  //     case NYA_TOKEN_SYMBOL: {
  //       printf("SYMBOL '%c' at %u:%u\n", token->symbol, token->line_number, token->char_number);
  //       break;
  //     }
  //
  //     case NYA_TOKEN_IDENT: {
  //       printf(
  //           "IDENT: %.*s at %u:%u\n",
  //           token->length,
  //           lexer.source + token->source_location,
  //           token->line_number,
  //           token->char_number
  //       );
  //       break;
  //     }
  //
  //     case NYA_TOKEN_NUMBER_INTEGER: {
  //       printf(
  //           "NUMBER INTEGER: %.*s at %u:%u\n",
  //           token->length,
  //           lexer.source + token->source_location,
  //           token->line_number,
  //           token->char_number
  //       );
  //       break;
  //     }
  //
  //     case NYA_TOKEN_NUMBER_FLOAT: {
  //       printf(
  //           "NUMBER FLOAT: %.*s at %u:%u\n",
  //           token->length,
  //           lexer.source + token->source_location,
  //           token->line_number,
  //           token->char_number
  //       );
  //       break;
  //     }
  //
  //     default: printf("UNKNOWN TOKEN TYPE %u at %u:%u\n", token->type, token->line_number, token->char_number);
  //     break;
  //   }
  // }

  nya_lexer_destroy(&lexer);
  return 0;
}
