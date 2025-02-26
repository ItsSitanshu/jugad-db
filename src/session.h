#ifndef SESSION_H
#define SESSION_H

#include <stdbool.h>

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
  char columns[10][64]; // Max 10 columns
  char values[10][64];  // Max 10 values
  int column_count;
  char where_clause[128];  // WHERE conditions
  char order_by[64];       // ORDER BY column
  bool is_odr_asc; 
  int limit;               // LIMIT count
} SQLCommand;

void start_session();

SQLCommand parse_sql(const char *command);

bool execute_command(SQLCommand *cmd);

#endif
