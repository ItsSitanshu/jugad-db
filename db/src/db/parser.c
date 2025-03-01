#include "parser.h"

Parser* parser_init(Lexer* lexer) {
  Parser* parser = malloc(sizeof(Parser));
  
  parser->lexer = lexer;
  parser->cur = NULL;
  parser->exec = calloc(1, sizeof(JQLCommand));

  return parser;
}

void parser_reset(Parser* parser) {
  parser->cur = lexer_next_token(parser->lexer);
}

void parser_free(Parser* parser) {
  if (!parser) {
    return;
  }

  lexer_free(parser->lexer);

  parser = NULL;
}

JQLCommand parser_parse(Parser* parser) {
  JQLCommand command;
  command.type = CMD_UNKNOWN;
  memset(&command, 0, sizeof(JQLCommand));

  switch (parser->cur->type) {
    case TOK_CRT:
      return parser_parse_create_table(parser);
    default:
      REPORT_ERROR(parser->lexer, "SYE_UNSUPPORTED");
      return command;
  }
}

JQLCommand parser_parse_create_table(Parser *parser) {
  JQLCommand command;
  memset(&command, 0, sizeof(JQLCommand));
  command.type = CMD_CREATE;

  parser_consume(parser);

  if (parser->cur->type != TOK_TBL) {
    // SYNTAX ERROR [EXPECTED] TABLE AFTER CREATE
    REPORT_ERROR(parser->lexer, "SYE_E_TAFCR");
    return command;
  }

  parser_consume(parser);

  if (parser->cur->type != TOK_ID) {
    // SYNTAX ERROR [EXPECTED] TABLE NAME AFTER TABLE
    REPORT_ERROR(parser->lexer, "SYE_E_TNAFTA");
    return command;
  }

  strcpy(command.table, parser->cur->value);
  parser_consume(parser);



  if (parser->cur->type != TOK_LP) {
    // SYNTAX ERROR [EXPECTED] PAREN AFTER "TABLE"
    REPORT_ERROR(parser->lexer, "SYE_E_PRNAFDYNA", "TABLE");
    return command;
  }
  
  parser_consume(parser);

  // Parsing column definitions
  while (parser->cur->type == TOK_ID) {
    strcpy(command.columns[command.column_count], parser->cur->value);
    command.column_count++;
    parser_consume(parser);
    
    if (parser->cur->type == TOK_COM) {
      parser_consume(parser);
    } else if (parser->cur->type == TOK_RP) {
      break;
    } else {
      // SYNTAX ERROR [EXPECTED] CLOSING PAREN OR COMMA
      REPORT_ERROR(parser->lexer, "SYE_E_CPRORCOM");
      return command;
    }
  }

  if (parser->cur->type != TOK_RP) {
    // SYNTAX ERROR [EXPECTED] CLOSING PAREN OR COMMA
    REPORT_ERROR(parser->lexer, "SYE_E_CPR");
    return command;
  }

  parser_consume(parser);

  return command;
}

void parser_consume(Parser* parser) {
  if (parser->cur->type == TOK_EOF) {
    exit(0);
  }

  token_free(parser->cur);

  parser->cur = lexer_next_token(parser->lexer);
}