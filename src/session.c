#include "session.h"
#include "storage.h"
#include "jdl.h"

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
      // Placeholder for insert logic
      break;
    case CMD_SELECT:
      // Placeholder for select logic
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
      printf("Unknown command! Use .help for valid commands and .quit to exit.\n");
      return false;
  }

  return true;
}

void process_dot_commands(const char *command, FILE **db_file, char *selected_db) {
  if (strcmp(command, ".clear") == 0) {
    printf("\033[H\033[J");
    return;
  }

  if (strcmp(command, ".quit") == 0) {
    printf("Exiting...\n");
    exit(0);
  }

  if (strcmp(command, ".help") == 0) {
    execute_command("", &(SQLCommand){.type = CMD_HELP});
    return;
  }

  if (strncmp(command, ".schema ", 8) == 0) {
    char *db_name = command + 8;

    if (strlen(db_name) < 4 || strcmp(db_name + strlen(db_name) - 4, ".jdb") != 0) {
      printf("Error: Invalid file. Please provide a .jdb file.\n");
      return;
    }
  
    if (*db_file) fclose(*db_file);
    *db_file = fopen(db_name, "a+");
  
    if (!*db_file) {
      printf("Error: Could not open or create database file '%s'.\n", db_name);
      return;
    }
    
    strcpy(selected_db, db_name);
    printf("-- Schema File selected: %s\n", selected_db);
  }

  if ((strncmp(command, ".exec ", 6) == 0) && selected_db) {
    char *filename = command + 6;
    FILE *script = fopen(filename, "r");

    if (!script) {
      printf("Error: Could not open script file '%s'.\n", filename);
      return;
    }

    fseek(script, 0, SEEK_END);
    long length = ftell(script);
    fseek(script, 0, SEEK_SET);
    
    char *buffer = malloc(length + 1);
    if (!buffer) {
      printf("Error: Memory allocation failed.\n");
      fclose(script);
      return;
    }

    fread(buffer, 1, length, script);
    buffer[length] = '\0'; 
    fclose(script);

    char *sql_command = strtok(buffer, ";");
    while (sql_command) {
      while (*sql_command && isspace(*sql_command)) sql_command++; 
      if (*sql_command) { 
        SQLCommand parsed_command = parse_sql(sql_command);
        execute_command(selected_db, &parsed_command);
      }
      sql_command = strtok(NULL, ";");
    }

    free(buffer);
  }
}

void start_session() {
  char command[256];
  FILE *db_file = NULL;
  char selected_db[256] = "";
  char cwd[256] = "";

  if (!getcwd(cwd, sizeof(cwd))) {
    strcpy(cwd, "unknown/"); 
  }

  printf("-- No schema file was selected '.schema <NAME.jdb>' to select or create a SCHEMA FILE\n");

  while (1) {
    printf(_BLU "%s:"_R _GRN"jugad-db@["_MAG"%s"_R _GRN"] "_R"$ ", cwd, selected_db[0] ? selected_db : "~");
    if (!fgets(command, sizeof(command), stdin)) break;

    command[strcspn(command, "\n")] = 0;
    
    if (command[0] == '.') {
      process_dot_commands(command, &db_file, selected_db);
      continue;
    }
    
    if (!db_file) {
      printf("Error: No database selected. Use '.schema <database.jdb>' to select one.\n");
      continue;
    }
    SQLCommand parsed_command = parse_sql(command);
    execute_command(selected_db, &parsed_command);
  }

  printf("Session closed.\n");
  if (db_file) fclose(db_file);
}