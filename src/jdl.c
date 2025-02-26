
#include "jdl.h"

void create_table (
  const char *db,
  const char *table_name,
  Column *columns,
  int column_count
) {
  FILE *file = fopen(db, "ab");

  if (!file) {
    printf("Error: Could not open database file.\n");
    return;
  }
  
  TableSchema schema;
  strncpy(schema.table_name, table_name, 64);
  schema.column_count = column_count;
  
  for (int i = 0; i < column_count; i++) {
    strncpy(schema.columns[i].name, columns[i].name, 64);
    schema.columns[i].type = columns[i].type;
    schema.columns[i].size = columns[i].size;
  }

  fwrite(&schema, sizeof(TableSchema), 1, file);
  fclose(file);
  
  printf("Table '%s' created successfully.\n", table_name);
}