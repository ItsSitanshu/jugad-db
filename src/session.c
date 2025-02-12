#include "session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *trim(char *str) {
  while (*str == ' ') str++;
  
  if (*str == '\0') return str;
  
  char *end = str + strlen(str) - 1;
  
  while (end > str && (*end == ' ' || *end == '\n' || *end == '\r')) end-- {
    *(end + 1) = '\0';
  }

    return str;
}

SQLCommand parse_sql(const char *command) {
  SQLCommand cmd;
  cmd.type = CMD_UNKNOWN;
  cmd.column_count = 0;
  cmd.limit = -1;
  memset(cmd.table, 0, sizeof(cmd.table));
  memset(cmd.columns, 0, sizeof(cmd.columns));
  memset(cmd.values, 0, sizeof(cmd.values));
  memset(cmd.where_clause, 0, sizeof(cmd.where_clause));
  memset(cmd.order_by, 0, sizeof(cmd.order_by));

  char query[256];
  strcpy(query, command);
  char *token = strtok(query, " ");
  if (!token) return cmd;

  if (strcasecmp(token, "INSERT") == 0) {
      cmd.type = CMD_INSERT;
      strtok(NULL, " "); 
      strcpy(cmd.table, strtok(NULL, " "));
      strtok(NULL, "(");
      char *col = strtok(NULL, ")");
      char *val;

      if (col) {
          char *col_token = strtok(col, ",");
          while (col_token) {
              strcpy(cmd.columns[cmd.column_count], trim(col_token));
              col_token = strtok(NULL, ",");
              cmd.column_count++;
          }
      }

      strtok(NULL, "(");
      val = strtok(NULL, ")");
      if (val) {
          char *val_token = strtok(val, ",");
          int i = 0;
          while (val_token && i < cmd.column_count) {
              strcpy(cmd.values[i], trim(val_token));
              val_token = strtok(NULL, ",");
              i++;
          }
      }
  } else if (strcasecmp(token, "SELECT") == 0) {
      cmd.type = CMD_SELECT;
      token = strtok(NULL, " ");
      int col_index = 0;
      while (token && strcasecmp(token, "FROM") != 0) {
          if (strcmp(token, "*") != 0) {
              strcpy(cmd.columns[col_index], trim(token));
              col_index++;
          }
          token = strtok(NULL, " ");
      }
      cmd.column_count = col_index;

      token = strtok(NULL, " ");
      if (token) strcpy(cmd.table, trim(token));

      while ((token = strtok(NULL, " ")) != NULL) {
          if (strcasecmp(token, "WHERE") == 0) {
              char *where = strtok(NULL, " ");
              if (where) strcpy(cmd.where_clause, trim(where));
          } else if (strcasecmp(token, "ORDER") == 0) {
              strtok(NULL, " ");
              char *order_col = strtok(NULL, " ");
              if (order_col) strcpy(cmd.order_by, trim(order_col));
          } else if (strcasecmp(token, "LIMIT") == 0) {
              char *limit_val = strtok(NULL, " ");
              if (limit_val) cmd.limit = atoi(limit_val);
          }
      }
  }

  return cmd;
}

bool execute_command(SQLCommand *cmd) {
  if (cmd->type == CMD_INSERT) {
    printf("Executing INSERT into %s\n", cmd->table);
    for (int i = 0; i < cmd->column_count; i++) {
      printf("  %s = %s\n", cmd->columns[i], cmd->values[i]);
    }
  } else if (cmd->type == CMD_SELECT) {
    printf("Executing SELECT on %s\n", cmd->table);
    if (cmd->column_count > 0) {
      printf("  Columns: ");
      for (int i = 0; i < cmd->column_count; i++) {
        printf("%s ", cmd->columns[i]);
      }
      printf("\n");
    }

    if (strlen(cmd->where_clause) > 0) {
      printf("  WHERE %s\n", cmd->where_clause);
    }

    if (strlen(cmd->order_by) > 0) {
      printf("  ORDER BY %s\n", cmd->order_by);
    }

    if (cmd->limit > 0) {
      printf("  LIMIT %d\n", cmd->limit);
    }
  } else {
    printf("Unknown command\n");
  }

  return true;
}

void start_session() {
  char command[256];
  printf("Welcome to Jugad-DB! Type SQL commands or EXIT to quit.\n");

  while (1) {
    printf("jugad-db> ");
    if (!fgets(command, sizeof(command), stdin)) break;

    if (strcasecmp(trim(command), "EXIT") == 0) break;

    SQLCommand parsed_command = parse_sql(command);
    execute_command(&parsed_command);
  }

  printf("Session closed.\n");
}
