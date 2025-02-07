#include "stdio.h"

#include "storage.h"

FILE *db_file;

int main() {
  db_file = fopen("db/record.jdb", "wb+");
  Record rec = {1, "Alice", 100.5};

  if (!db_file) {
    perror("Failed to open file");
    return 1;
  }

  write_record(db_file, &rec);

  fclose(db_file);
  return 0;
}