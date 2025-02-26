#include "session.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 

char *trim(char *str) {
  while (*str == ' ') str++;
  
  if (*str == '\0') return str;
  
  char *end = str + strlen(str) - 1;
  
  while (end > str && (*end == ' ' || *end == '\n' || *end == '\r')) {
    *(end + 1) = '\0';
    end--;
  }

  return str;
}

void trim_leading_spaces(char *str) {
  while (isspace((unsigned char)*str)) str++;  // Move pointer past spaces
  memmove(str - (str - str), str, strlen(str) + 1);
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

  if (strcmp(command, ".quit") == 0 || strncmp(command, ".quit", 5) == 0) {
    printf("Exiting...\n");
    exit(0);
  }

  if (strcmp(command, ".help") == 0 || strncmp(command, ".help", 5) == 0) {
    cmd.type = CMD_HELP;
    return cmd;
  }

  char query[256];
  strncpy(query, command, sizeof(query) - 1);
  query[sizeof(query) - 1] = '\0';  // Ensure null termination
  trim_leading_spaces(query);  // Fix leading space issue

  char *token = strtok(query, " ");
  if (!token) return cmd;

  if (strcasecmp(token, "CREATE") == 0) {
    token = strtok(NULL, " ");
    if (token && strcasecmp(token, "TABLE") == 0) {
      cmd.type = CMD_CREATE;
      strcpy(cmd.table, strtok(NULL, " (")); 
      
      char *col_definitions = strtok(NULL, ")");
      if (col_definitions) {
        char *col_token = strtok(col_definitions, ",");
        while (col_token) {
          strcpy(cmd.columns[cmd.column_count], trim(col_token));
          col_token = strtok(NULL, ",");
          cmd.column_count++;
        }
      }
    }
  }

  if (strcasecmp(token, "INSERT") == 0) {
    cmd.type = CMD_INSERT;
    strtok(NULL, " ");  // Skip "INTO"
    strcpy(cmd.table, strtok(NULL, " "));  // Get table name

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
  }

  else if (strcasecmp(token, "SELECT") == 0) {
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
      } else if (strcasecmp(token, "ODR") == 0) {
        strtok(NULL, " "); 
        char *order_col = strtok(NULL, " ");
        if (order_col) strcpy(cmd.order_by, trim(order_col));
        
        char *sort_order = strtok(NULL, " ");
        if (sort_order) {
          if (strcasecmp(sort_order, "DSRT") == 0) {
            cmd.is_odr_asc = false;
          } else if (strcasecmp(sort_order, "ASRT") == 0) {
            cmd.is_odr_asc = true;
          }
        }
      } else if (strcasecmp(token, "LIMIT") == 0) {
        char *limit_val = strtok(NULL, " ");
        if (limit_val) cmd.limit = atoi(limit_val);
      }
    }
  }

  return cmd;
}

bool execute_command(SQLCommand *cmd) {
  switch (cmd->type) {
    case CMD_CREATE:
      printf("Creating table %s\n", cmd->table);
      printf("Columns:\n");
      for (int i = 0; i < cmd->column_count; i++) {
        printf("  - %s\n", cmd->columns[i]);
      }
      break;

    case CMD_INSERT:
      printf("Executing INSERT into %s\n", cmd->table);
      for (int i = 0; i < cmd->column_count; i++) {
        printf("\t  %s = %s\n", cmd->columns[i], cmd->values[i]);
      }
      break;

    case CMD_SELECT:
      printf("Executing SELECT on %s\n", cmd->table);
      if (cmd->column_count > 0) {
        printf("\tColumns: \n");
        for (int i = 0; i < cmd->column_count; i++) {
          printf("%s ", cmd->columns[i]);
        }
        printf("\n");
      }

      if (cmd->where_clause && strlen(cmd->where_clause) > 0) {
        printf("\tWHERE %s\n", cmd->where_clause);
      }

      if (cmd->order_by && strlen(cmd->order_by) > 0) {
        printf("\tORDER BY %s\n", cmd->order_by);
      }

      if (cmd->limit > 0) {
        printf("\tLIMIT %d\n", cmd->limit);
      }
      break;

    case CMD_HELP:
      printf(
        "\nJUGAD-DB COMMAND REFERENCE\n"

        "GENERAL COMMANDS\n"
        "  .quit                Exit the database system.\n"
        "  .help                Show this help menu.\n\n"

        "DATA DEFINITION LANGUAGE (JDL)\n"
        "  CREATE TABLE <name> (<column definitions>);\n"
        "      Define a new table.\n"
        "  ALTER TABLE <name> ADD COLUMN <column definition>;\n"
        "      Modify an existing table.\n\n"

        "DATA QUERY / MODIFICATION LANGUAGE (DQL)\n"
        "  INSERT INTO <table> (<columns>) VALUES (<values>);\n"
        "      Insert new rows into a table.\n"
        "  UPDATE <table> SET <column> = <value> WHERE <condition>;\n"
        "      Update existing records.\n"
        "  DELETE FROM <table> WHERE <condition>;\n"
        "      Delete records.\n"

        "  SELECT <columns> FROM <table> WHERE <condition> ORDER BY <column> [ASRT|DSRT] LIMIT <n>;\n"
        "      Retrieve data from a table.\n"
        "  SELECT <columns> FROM <table> JOIN <table> ON <condition>;\n"
        "      Perform joins between tables.\n"
        "  SELECT <column>, SUM(<column>) AS <alias> FROM <table> GROUP BY <column>;\n"
        "      Aggregate data.\n\n"

        "SYNTAX NOTES\n"
        "  - PRM  : Primary Key\n"
        "  - FRN REF <table>(<column>) : Foreign Key\n"
        "  - ODR  : ORDER BY\n"
        "  - ASRT : Ascending order (default)\n"
        "  - DSRT : Descending order\n"
        "  - LIM  : LIMIT rows returned\n\n"

        "For further details and examples, refer to documentation and test/ .jql/.jcl files.\n\n"
      );
      break;

    default:
      printf("Unknown command! Use .help to obtain a list of valid commands and .quit to kill this session\n");
      return false;
  }

  return true;
}

void start_session() {
  char command[256];
  FILE *db_file = NULL;
  char selected_db[256] = "";
  char cwd[256] = "";

  if (!getcwd(cwd, sizeof(cwd))) {
    strcpy(cwd, "ukwn/"); 
  }

  printf("Welcome to Jugad-DB! Type '.schema <DATABASE_NAME.jdb>' to select a SCHEMA FILE\n");

  while (1) {
    printf("%s: jugad-db@[%s] $ ", cwd, selected_db[0] ? selected_db : "~");
    
    if (!fgets(command, sizeof(command), stdin)) break;

    command[strcspn(command, "\n")] = 0;

    if (strncmp(command, ".schema ", 8) == 0) {
      char *db_name = command + 8; 
  
      if (strlen(db_name) < 4 || strcmp(db_name + strlen(db_name) - 4, ".jdb") != 0) {
        printf("Error: Invalid file. Please provide a .jdb file.\n");
        continue;
      }
  
      int db_existed = access(db_name, F_OK) == 0;
      
      if (db_file) fclose(db_file);

      db_file = fopen(db_name, "w");
      if (!db_file) {
        printf("Error: Could not open or create database file '%s'.\n", db_name);
        continue;
      }
  
      strcpy(selected_db, db_name);

      printf("Database %s: %s\n", db_existed ? "entered" : "created", selected_db);
      continue;
    }
  

    if (!db_file) {
      printf("Error: No database selected. Use 'ENTER <database.jdb>' to select one.\n");
      continue;
    }

    SQLCommand parsed_command = parse_sql(command);
    execute_command(&parsed_command);
  }

  printf("Session closed.\n");

  if (db_file) fclose(db_file);
}