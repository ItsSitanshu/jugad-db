#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TABLE_NAME 32
#define MAX_SCHEMA 128
#define BTREE_ORDER 4  

typedef enum { IO_READ, IO_WRITE, IO_APPEND } IOMode;

typedef struct IO {
  FILE* file;
  char* buffer;
  size_t buf_size;
  size_t buf_capacity;
} IO;

typedef struct {
  char table_name[MAX_TABLE_NAME];
  char schema[MAX_SCHEMA];
  long root_offset;  // B-Tree root offset
} TableMetadata;

typedef struct {
  int keys[BTREE_ORDER - 1];
  long children[BTREE_ORDER];
  int num_keys;
  int is_leaf;
} BTreeNode;

IO* io_init(const char* filename, IOMode mode, size_t buffer_capacity);
void io_close(IO* io);
void io_flush(IO* io);
void io_write(IO* io, const void* data, size_t size);
size_t io_read(IO* io, void* buffer, size_t size);

void io_write_metadata(IO* io, const char* table_name, const char* schema);
TableMetadata* io_read_metadata(IO* io, const char* table_name);

int io_write_record(IO* io, int key, void* record, size_t record_size);
int io_read_record(IO* io, int key, void* buffer, size_t record_size);

void btree_insert(IO* io, TableMetadata* meta, int key, long record_offset);
long btree_search(IO* io, TableMetadata* meta, int key);

#endif // IO_H
