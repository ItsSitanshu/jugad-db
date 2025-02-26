#include "parser.h"

DataType get_data_type(const char *type_str) {
  if (strcasecmp(type_str, "INT") == 0) return TYPE_INT;
  if (strcasecmp(type_str, "SMALLINT") == 0) return TYPE_SMALLINT;
  if (strcasecmp(type_str, "BIGINT") == 0) return TYPE_BIGINT;
  if (strcasecmp(type_str, "FLOAT") == 0) return TYPE_FLOAT;
  if (strcasecmp(type_str, "DOUBLE") == 0) return TYPE_DOUBLE;
  if (strcasecmp(type_str, "BOOL") == 0) return TYPE_BOOL;
  if (strcasecmp(type_str, "CHAR") == 0) return TYPE_CHAR;
  if (strcasecmp(type_str, "VARCHAR") == 0) return TYPE_VARCHAR;
  if (strcasecmp(type_str, "TEXT") == 0) return TYPE_TEXT;
  if (strcasecmp(type_str, "DATE") == 0) return TYPE_DATE;
  if (strcasecmp(type_str, "TIME") == 0) return TYPE_TIME;
  if (strcasecmp(type_str, "DATETIME") == 0) return TYPE_DATETIME;
  if (strcasecmp(type_str, "TIMESTAMP") == 0) return TYPE_TIMESTAMP;
  if (strcasecmp(type_str, "BINARY") == 0) return TYPE_BINARY;
  if (strcasecmp(type_str, "DECIMAL") == 0) return TYPE_DECIMAL;
  if (strcasecmp(type_str, "UUID") == 0) return TYPE_UUID;
  if (strcasecmp(type_str, "OBJECT") == 0) return TYPE_OBJECT;
  if (strcasecmp(type_str, "ARRAY") == 0) return TYPE_ARRAY;
}

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


int parse_create_table(const char *query, SQLCommand *cmd) {
  cmd->type = CMD_CREATE;
  char query_copy[512];
  strncpy(query_copy, query, sizeof(query_copy) - 1);
  query_copy[sizeof(query_copy) - 1] = '\0'; 

  char *token = strtok(query_copy, " ("); // Ensure token is declared
  if (!token) return 0;
  strncpy(cmd->table, trim(token), sizeof(cmd->table) - 1);
  cmd->table[sizeof(cmd->table) - 1] = '\0';

  cmd->column_count = 0;
  
  char *col_definitions = strtok(NULL, ")");
  if (!col_definitions) return 0; // Ensure col_definitions is valid

  char *col_token = strtok(col_definitions, ",");
  while (col_token && cmd->column_count < MAX_COLUMNS) {
    col_token = trim(col_token);

    char *col_name = strtok(col_token, " ");
    if (!col_name) return 0;
    strncpy(cmd->columns.create[cmd->column_count].name, col_name, sizeof(cmd->columns.create[cmd->column_count].name) - 1);
    cmd->columns.create[cmd->column_count].name[sizeof(cmd->columns.create[cmd->column_count].name) - 1] = '\0';

    char *type_token = strtok(NULL, " ");
    if (!type_token) return 0;
    cmd->columns.create[cmd->column_count].type = get_data_type(type_token);

    switch (cmd->columns.create[cmd->column_count].type) {
      case TYPE_CHAR:
      case TYPE_VARCHAR:
        type_token = strtok(NULL, " ");
        if (type_token && type_token[0] == '(') {
          cmd->columns.create[cmd->column_count].size = atoi(type_token + 1);
        } else {
          cmd->columns.create[cmd->column_count].size = 255;
        }
        break;
      default:
        cmd->columns.create[cmd->column_count].size = 0;
    }

    cmd->column_count++;
    col_token = strtok(NULL, ",");
  }

  return 1; 
}

SQLCommand parse_sql(const char *command) {
  SQLCommand cmd;

  cmd.type = CMD_UNKNOWN;
  cmd.column_count = 0;
  cmd.limit = -1;
  memset(cmd.table, 0, sizeof(cmd.table));
  // memset(cmd.columns, 0, sizeof(cmd.columns));
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
      token = strtok(NULL, " ");
      parse_create_table(token, &cmd);
    }
  }

  // if (strcasecmp(token, "INSERT") == 0) {
  //   cmd.type = CMD_INSERT;
  //   strtok(NULL, " ");  // Skip "INTO"
  //   strcpy(cmd.table, strtok(NULL, " "));  // Get table name

  //   strtok(NULL, "(");
  //   char *col = strtok(NULL, ")");
  //   char *val;

  //   if (col) {
  //     char *col_token = strtok(col, ",");
  //     while (col_token) {
  //       strcpy(cmd.columns[cmd.column_count], trim(col_token));
  //       col_token = strtok(NULL, ",");
  //       cmd.column_count++;
  //     }
  //   }

  //   strtok(NULL, "(");
  //   val = strtok(NULL, ")");
  //   if (val) {
  //     char *val_token = strtok(val, ",");
  //     int i = 0;
  //     while (val_token && i < cmd.column_count) {
  //       strcpy(cmd.values[i], trim(val_token));
  //       val_token = strtok(NULL, ",");
  //       i++;
  //     }
  //   }
  // }

  // else if (strcasecmp(token, "SELECT") == 0) {
  //   cmd.type = CMD_SELECT;
  //   token = strtok(NULL, " ");
  //   int col_index = 0;

  //   while (token && strcasecmp(token, "FROM") != 0) {
  //     if (strcmp(token, "*") != 0) {
  //       strcpy(cmd.columns[col_index], trim(token));
  //       col_index++;
  //     }
  //     token = strtok(NULL, " ");
  //   }

  //   cmd.column_count = col_index;
  //   token = strtok(NULL, " ");
  //   if (token) strcpy(cmd.table, trim(token));

  //   while ((token = strtok(NULL, " ")) != NULL) {
  //     if (strcasecmp(token, "WHERE") == 0) {
  //       char *where = strtok(NULL, " ");
  //       if (where) strcpy(cmd.where_clause, trim(where));
  //     } else if (strcasecmp(token, "ODR") == 0) {
  //       strtok(NULL, " "); 
  //       char *order_col = strtok(NULL, " ");
  //       if (order_col) strcpy(cmd.order_by, trim(order_col));
        
  //       char *sort_order = strtok(NULL, " ");
  //       if (sort_order) {
  //         if (strcasecmp(sort_order, "DSRT") == 0) {
  //           cmd.is_odr_asc = false;
  //         } else if (strcasecmp(sort_order, "ASRT") == 0) {
  //           cmd.is_odr_asc = true;
  //         }
  //       }
  //     } else if (strcasecmp(token, "LIMIT") == 0) {
  //       char *limit_val = strtok(NULL, " ");
  //       if (limit_val) cmd.limit = atoi(limit_val);
  //     }
  //   }
  // }

  return cmd;
}