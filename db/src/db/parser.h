#ifndef JQL_COMMAND_H
#define JQL_COMMAND_H

#include "lexer.h"

typedef enum {
  CMD_SELECT,
  CMD_INSERT,
  CMD_UPDATE,
  CMD_DELETE,
  CMD_CREATE,
  CMD_DROP,
  CMD_ALTER,
  CMD_JOIN,
  CMD_TRUNCATE,
  CMD_REPLACE,
  CMD_MERGE,
  CMD_CALL,
  CMD_EXPLAIN,
  CMD_SHOW,
  CMD_DESCRIBE,
  CMD_SET,
  CMD_COMMIT,
  CMD_ROLLBACK,
  CMD_SAVEPOINT,
  CMD_GRANT,
  CMD_REVOKE,
  CMD_LOCK,
  CMD_UNLOCK,
  CMD_ANALYZE,
  CMD_UNKNOWN
} JQLCommandType;

typedef enum {
  AST_COMMAND,
  AST_TABLE,
  AST_COLUMN,
  AST_VALUE,
  AST_CONDITION,
  AST_OPERATOR,
  AST_FUNCTION,
  AST_JOIN,
  AST_ORDER,
  AST_GROUP,
  AST_LIMIT,
  AST_TRANSACTION
} ASTNodeType;

typedef struct ASTNode {
  ASTNodeType type;
  char value[MAX_IDENTIFIER_LEN];
  struct ASTNode *left;
  struct ASTNode *right;
} ASTNode;

typedef struct ExecutionOrder {
  int step;
  char operation[MAX_IDENTIFIER_LEN];
  struct ExecutionOrder *next;
} ExecutionOrder;

typedef struct {
  JQLCommandType type;  // Type of the command
  ASTNode* ast_root;  // Pointer to the AST root node
  ExecutionOrder* execution_order; // Execution order of the query

  char table[MAX_IDENTIFIER_LEN]; 
  char columns[NO_OF_KEYWORDS][MAX_IDENTIFIER_LEN]; 
  int column_count;  
  char values[NO_OF_KEYWORDS][MAX_IDENTIFIER_LEN];  
  int value_count; 
  char conditions[MAX_IDENTIFIER_LEN];  // WHERE conditions as a string
  char order_by[MAX_IDENTIFIER_LEN];  // ORDER BY clause
  char group_by[MAX_IDENTIFIER_LEN];  // GROUP BY clause
  char having[MAX_IDENTIFIER_LEN];  // HAVING clause
  char limit[MAX_IDENTIFIER_LEN];  // LIMIT clause
  char offset[MAX_IDENTIFIER_LEN]; 
  char join_table[MAX_IDENTIFIER_LEN]; 
  char join_condition[MAX_IDENTIFIER_LEN]; 
  char constraints[MAX_IDENTIFIER_LEN];  
  char indexes[MAX_IDENTIFIER_LEN]; 
  char alias[MAX_IDENTIFIER_LEN]; 
  char functions[NO_OF_KEYWORDS][MAX_IDENTIFIER_LEN]; 
  int function_count; 
  char transaction[MAX_IDENTIFIER_LEN]; 
} JQLCommand;

typedef struct {
  Lexer* lexer;
  Token* cur;
  JQLCommand** exec;
} Parser;

Parser* parser_init(Lexer* lexer);
void parser_free(Parser* parser);
void parser_reset(Parser* parser);

JQLCommand parser_parse(Parser* parser);

JQLCommand parser_parse_create_table(Parser *parser);

void parser_consume(Parser* parser);
ASTNode* parse_expression(Parser* parser);


#endif // JQL_COMMAND_H
