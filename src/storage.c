#include "storage.h"

void write_record(FILE *file, Record *rec) {
  fwrite(rec, sizeof(Record), 1, file);
  fflush(file);
}

void read_record(FILE *file, Record *rec) {
  fread(rec, sizeof(Record), 1, file);
}
