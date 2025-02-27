#include <stdio.h>

#include "parser.h"

int main(int argc, char* argv[]) {
  char* buffer = "SELECT user_id, SUM(amount) AS total_spent FROM orders GROUP BY user_id;";
  
  Lexer* lexer = lexer_init(buffer);
  Token* tok = lexer_next_token(lexer);

  while (tok->type != TOK_EOF) {
    printf("%d | %s\n", tok->type, tok->value);
    tok = lexer_next_token(lexer);
  }

  return 0;
}