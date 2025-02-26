#include "session.h"
#include "storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 

bool execute_command(const char *db, SQLCommand *cmd) {
  switch (cmd->type) {
    case CMD_CREATE:
      create_table(db, cmd->table, cmd->columns.create, cmd->column_count);
      break;
    case CMD_INSERT:
      printf("Executing INSERT into %s\n", cmd->table);
      for (int i = 0; i < cmd->column_count; i++) {
        printf("\t  %s = %s\n", cmd->columns.cols[i], cmd->values[i]);
      }
      break;

    case CMD_SELECT:
      printf("Executing SELECT on %s\n", cmd->table);
      if (cmd->column_count > 0) {
        printf("\tColumns: \n");
        for (int i = 0; i < cmd->column_count; i++) {
          printf("%s ", cmd->columns.cols[i]);
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
        "\n" _BLD "JUGAD-DB COMMAND REFERENCE\n" _R
    
        _YLW "GENERAL COMMANDS\n" _R
        _CY "  .quit                " _R "Exit the database system.\n"
        _CY "  .help                " _R "Show this help menu.\n\n"
        _CY "  .schema <name>       " _R "Selects schema file.\n\n"
        _CY "  .clear               " _R "Clears screen.\n\n"
    
        _YLW "DATA DEFINITION LANGUAGE (JDL)\n" _R
        _CY "  CREATE TABLE <name> (<column definitions>);\n" _R
        "      Define a new table.\n"
        _CY "  ALTER TABLE <name> ADD COLUMN <column definition>;\n" _R
        "      Modify an existing table.\n\n"
    
        _YLW "DATA QUERY / MODIFICATION LANGUAGE (DQL)\n" _R
        _CY "  INSERT INTO <table> (<columns>) VALUES (<values>);\n" _R
        "      Insert new rows into a table.\n"
        _CY "  UPDATE <table> SET <column> = <value> WHERE <condition>;\n" _R
        "      Update existing records.\n"
        _CY "  DELETE FROM <table> WHERE <condition>;\n" _R
        "      Delete records.\n\n"
    
        _CY "  SELECT <columns> FROM <table> WHERE <condition> ORDER BY <column> [ASRT|DSRT] LIMIT <n>;\n" _R
        "      Retrieve data from a table.\n"
        _CY "  SELECT <columns> FROM <table> JOIN <table> ON <condition>;\n" _R
        "      Perform joins between tables.\n"
        _CY "  SELECT <column>, SUM(<column>) AS <alias> FROM <table> GROUP BY <column>;\n" _R
        "      Aggregate data.\n\n"
    
        _YLW "SYNTAX NOTES\n" _R
        "  - " _BLD "PRM  " _R ": Primary Key\n"
        "  - " _BLD "FRN REF <table>(<column>) " _R ": Foreign Key\n"
        "  - " _BLD "ODR  " _R ": ORDER BY\n"
        "  - " _BLD "ASRT " _R ": Ascending order (default)\n"
        "  - " _BLD "DSRT " _R ": Descending order\n"
        "  - " _BLD "LIM  " _R ": LIMIT rows returned\n\n"
    
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

  printf("Welcome to Jugad-DB! Type " _BLD "'.schema <NAME.jdb>'" _R " to select a" _BLD " SCHEMA FILE\n");

  while (1) {
    printf(_R _GRN "%s" _R ": " _BLU "jugad-db@[%s]" _R " $ ", 
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
      execute_command("", &(SQLCommand){type: CMD_HELP});
    }  

    if (strncmp(command, ".schema ", 8) == 0) {
      char *db_name = command + 8; 

      if (strlen(db_name) < 4 || strcmp(db_name + strlen(db_name) - 4, ".jdb") != 0) {
        printf(_RED "Error: Invalid file. Please provide a .jdb file.\n" _R);
        continue;
      }

      int db_existed = access(db_name, F_OK) == 0;
      
      if (db_file) fclose(db_file);

      db_file = fopen(db_name, "a+");
      if (!db_file) {
        printf(_RED "Error: Could not open or create database file '%s'.\n" _R, db_name);
        continue;
      }

      strcpy(selected_db, db_name);

      printf(_GRN "Database %s: %s\n" _R, db_existed ? "entered" : "created", selected_db);
      continue;
    }

    if (!db_file) {
      printf(_RED "Error: No database selected. Use '.schema <database.jdb>' to select one.\n" _R);
      continue;
    }

    SQLCommand parsed_command = parse_sql(command);
    execute_command(selected_db, &parsed_command);
  }

  printf(_MAG "Session closed.\n" _R);

  if (db_file) fclose(db_file);
}