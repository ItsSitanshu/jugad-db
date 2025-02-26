#ifndef PARSER_H
#define PARSER_H

#include "storage.h"

#include <stdbool.h>
#include <ctype.h>

#define MAX_COLUMNS 64

typedef enum {
  CMD_INSERT,
  CMD_SELECT,
  CMD_UPDATE,
  CMD_DELETE,
  CMD_UNKNOWN,
  CMD_HELP,
  CMD_CREATE
} CommandType;

typedef struct {
  CommandType type;
  char table[64];
  union {
    Column* create;
    char cols[MAX_COLUMNS][64]; // Max 10 columns
  } columns;
  char values[10][64];  // Max 10 values
  int column_count;
  char where_clause[128];  // WHERE conditions
  char order_by[64];       // ORDER BY column
  bool is_odr_asc; 
  int limit;               // LIMIT count
} SQLCommand;


char *trim(char *str);
void trim_leading_spaces(char *str);
DataType get_data_type(const char *type_str);

int parse_create_table(const char *query, SQLCommand *cmd);

SQLCommand parse_sql(const char *command);


#endif // PARSER_H