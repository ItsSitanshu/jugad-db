#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
  TYPE_INT,         // 4-byte integer
  TYPE_SMALLINT,    // 2-byte integer
  TYPE_BIGINT,      // 8-byte integer
  TYPE_FLOAT,       // 4-byte floating point
  TYPE_DOUBLE,      // 8-byte floating point
  TYPE_BOOL,        // Boolean (true/false)
  TYPE_CHAR,        // Fixed-length character
  TYPE_VARCHAR,     // Variable-length string
  TYPE_TEXT,        // Large text storage
  TYPE_DATE,        // Date (YYYY-MM-DD)
  TYPE_TIME,        // Time (HH:MM:SS)
  TYPE_DATETIME,    // Date + Time (YYYY-MM-DD HH:MM:SS)
  TYPE_TIMESTAMP,   // UNIX timestamp
  TYPE_BINARY,      // Raw binary data (BLOB)
  TYPE_DECIMAL,     // Fixed-precision decimal
  TYPE_UUID,        // Universally Unique Identifier (UUID)
  TYPE_OBJECT,      // Key-value pairs
  TYPE_ARRAY        // Array of a specific type
} DataType;

typedef struct Field;

typedef struct {
  char* key;
  struct Field* value;
} ObjectField;

typedef struct {
  ObjectField *fields;
  int count;
} ObjectType;

typedef struct {
  DataType type;
  struct Field *elements;
  int count;
} ArrayType;

typedef struct {
  DataType type;
  void* value;
  size_t size;
} Field;

typedef struct {
  int id;
  Field *fields;
} Record;

ObjectField create_object_field(const char *key, Field *value);
ObjectType create_object(int field_count);
void free_object(ObjectType *obj);

ArrayType create_int_array(int *values, int count);
void free_array(ArrayType *arr);
void write_record(FILE *file, Record *rec);
void read_record(FILE *file, Record *rec);

#endif
