#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  NYA_Lexer lexer = nya_lexer_new("a+b\nc \"2134.34 = .234. asdf! __asd__a __ sad__");
  nya_lexer_run(&lexer);

  nya_array_foreach (&lexer.tokens, token) {
    switch (token->type) {
      case NYA_TOKEN_SYMBOL: {
        printf("SYMBOL '%c' at %u:%u\n", token->symbol, token->line_number, token->char_number);
        break;
      }
      case NYA_TOKEN_IDENT: {
        printf(
            "IDENT: %.*s at %u:%u\n",
            token->length,
            lexer.source + token->source_location,
            token->line_number,
            token->char_number
        );
        break;
      }
      case NYA_TOKEN_NUMBER_INTEGER: {
        printf(
            "NUMBER INTEGER: %.*s at %u:%u\n",
            token->length,
            lexer.source + token->source_location,
            token->line_number,
            token->char_number
        );
        break;
      }
      case NYA_TOKEN_NUMBER_FLOAT: {
        printf(
            "NUMBER FLOAT: %.*s at %u:%u\n",
            token->length,
            lexer.source + token->source_location,
            token->line_number,
            token->char_number
        );
        break;
      }
      default: printf("UNKNOWN TOKEN TYPE %u at %u:%u\n", token->type, token->line_number, token->char_number); break;
    }
  }

  nya_lexer_destroy(&lexer);
  return 0;
}
