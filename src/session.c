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
      strtok(NULL, "(");   
      
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
        "\n" COLOR_BOLD "JUGAD-DB COMMAND REFERENCE\n" COLOR_RESET
    
        COLOR_YELLOW "GENERAL COMMANDS\n" COLOR_RESET
        COLOR_CYAN "  .quit                " COLOR_RESET "Exit the database system.\n"
        COLOR_CYAN "  .help                " COLOR_RESET "Show this help menu.\n\n"
        COLOR_CYAN "  .schema <name>       " COLOR_RESET "Selects schema file.\n\n"
        COLOR_CYAN "  .clear               " COLOR_RESET "Clears screen.\n\n"
    
        COLOR_YELLOW "DATA DEFINITION LANGUAGE (JDL)\n" COLOR_RESET
        COLOR_CYAN "  CREATE TABLE <name> (<column definitions>);\n" COLOR_RESET
        "      Define a new table.\n"
        COLOR_CYAN "  ALTER TABLE <name> ADD COLUMN <column definition>;\n" COLOR_RESET
        "      Modify an existing table.\n\n"
    
        COLOR_YELLOW "DATA QUERY / MODIFICATION LANGUAGE (DQL)\n" COLOR_RESET
        COLOR_CYAN "  INSERT INTO <table> (<columns>) VALUES (<values>);\n" COLOR_RESET
        "      Insert new rows into a table.\n"
        COLOR_CYAN "  UPDATE <table> SET <column> = <value> WHERE <condition>;\n" COLOR_RESET
        "      Update existing records.\n"
        COLOR_CYAN "  DELETE FROM <table> WHERE <condition>;\n" COLOR_RESET
        "      Delete records.\n\n"
    
        COLOR_CYAN "  SELECT <columns> FROM <table> WHERE <condition> ORDER BY <column> [ASRT|DSRT] LIMIT <n>;\n" COLOR_RESET
        "      Retrieve data from a table.\n"
        COLOR_CYAN "  SELECT <columns> FROM <table> JOIN <table> ON <condition>;\n" COLOR_RESET
        "      Perform joins between tables.\n"
        COLOR_CYAN "  SELECT <column>, SUM(<column>) AS <alias> FROM <table> GROUP BY <column>;\n" COLOR_RESET
        "      Aggregate data.\n\n"
    
        COLOR_YELLOW "SYNTAX NOTES\n" COLOR_RESET
        "  - " COLOR_BOLD "PRM  " COLOR_RESET ": Primary Key\n"
        "  - " COLOR_BOLD "FRN REF <table>(<column>) " COLOR_RESET ": Foreign Key\n"
        "  - " COLOR_BOLD "ODR  " COLOR_RESET ": ORDER BY\n"
        "  - " COLOR_BOLD "ASRT " COLOR_RESET ": Ascending order (default)\n"
        "  - " COLOR_BOLD "DSRT " COLOR_RESET ": Descending order\n"
        "  - " COLOR_BOLD "LIM  " COLOR_RESET ": LIMIT rows returned\n\n"
    
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
    strcpy(cwd, "unknown/"); 
  }

  printf("Welcome to Jugad-DB! Type " COLOR_BOLD "'.schema <NAME.jdb>'" COLOR_RESET " to select a" COLOR_BOLD " SCHEMA FILE\n");

  while (1) {
    printf(COLOR_RESET COLOR_MAGENTA "%s" COLOR_RESET ": " COLOR_CYAN "jugad-db@[%s]" COLOR_RESET " $ ", 
      cwd, selected_db[0] ? selected_db : "~");
    
    if (!fgets(command, sizeof(command), stdin)) break;

    command[strcspn(command, "\n")] = 0;

    if (strcmp(command, ".clear") == 0) {
      printf("\033[H\033[J");
      continue;
    }

    if (strcmp(command, ".quit") == 0 || strncmp(command, ".quit", 5) == 0) {
      printf("Exiting...\n");
      exit(0);
    }
  
    if (strcmp(command, ".help") == 0 || strncmp(command, ".help", 5) == 0) {
      execute_command(&(SQLCommand){type: CMD_HELP});
    }  

    if (strncmp(command, ".schema ", 8) == 0) {
      char *db_name = command + 8; 

      if (strlen(db_name) < 4 || strcmp(db_name + strlen(db_name) - 4, ".jdb") != 0) {
        printf(COLOR_RED "Error: Invalid file. Please provide a .jdb file.\n" COLOR_RESET);
        continue;
      }

      int db_existed = access(db_name, F_OK) == 0;
      
      if (db_file) fclose(db_file);

      db_file = fopen(db_name, "a+"); // Creates file if not exists
      if (!db_file) {
        printf(COLOR_RED "Error: Could not open or create database file '%s'.\n" COLOR_RESET, db_name);
        continue;
      }

      strcpy(selected_db, db_name);

      printf(COLOR_GREEN "Database %s: %s\n" COLOR_RESET, db_existed ? "entered" : "created", selected_db);
      continue;
    }

    if (!db_file) {
      printf(COLOR_RED "Error: No database selected. Use '.schema <database.jdb>' to select one.\n" COLOR_RESET);
      continue;
    }

    SQLCommand parsed_command = parse_sql(command);
    execute_command(&parsed_command);
  }

  printf(COLOR_MAGENTA "Session closed.\n" COLOR_RESET);

  if (db_file) fclose(db_file);
}