#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int id;
  char name[32];
  float balance;
} Record;

void write_record(FILE *file, Record *rec);
void read_record(FILE *file, Record *rec);

#endif
