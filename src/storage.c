#include "storage.h"

ObjectField create_object_field(const char *key, Field *value) {
  ObjectField field;
  field.key = strdup(key);
  field.value = malloc(sizeof(Field));
  memcpy(field.value, value, sizeof(Field));
  return field;
}

ObjectType create_object(int field_count) {
  ObjectType obj;
  obj.fields = malloc(sizeof(ObjectField) * field_count);
  obj.count = field_count;
  return obj;
}

void free_object(ObjectType *obj) {
  for (int i = 0; i < obj->count; i++) {
    free(obj->fields[i].key);
    free(obj->fields[i].value);
  }
  free(obj->fields);
}

// TODO: Implement array type
// ArrayType create_int_array(int *values, int count) {
//   ArrayType arr;
//   arr.type = TYPE_INT;
//   arr.count = count;
  
//   arr.elements = (Field *)malloc(sizeof(Field) * count);
//   if (!arr.elements) {
//       fprintf(stderr, "Memory allocation failed\n");
//       exit(1);
//   }

//   for (int i = 0; i < count; i++) {
//       arr.elements[i].type = TYPE_INT;
//       arr.elements[i].size = sizeof(int);
      
//       arr.elements[i].value = malloc(sizeof(int));
//       if (!arr.elements[i].value) {
//         fprintf(stderr, "Memory allocation failed\n");
//         exit(1);
//       }
      
//       memcpy(arr.elements[i].value, &values[i], sizeof(int));
//   }

//   return arr;
// }

// void free_array(ArrayType *arr) {
//     for (int i = 0; i < arr->count; i++) {
//         free(arr->elements[i].value);
//     }
//     free(arr->elements);
// }

void write_record(FILE *file, Record *rec) {
  fwrite(&rec->id, sizeof(int), 1, file);
  for (int i = 0; i < rec->fields->size; i++) {
    fwrite(&rec->fields[i].type, sizeof(DataType), 1, file);
    fwrite(&rec->fields[i].size, sizeof(size_t), 1, file);
    fwrite(rec->fields[i].value, rec->fields[i].size, 1, file);
  }
}

void read_record(FILE *file, Record *rec) {
  fread(&rec->id, sizeof(int), 1, file);
  for (int i = 0; i < rec->fields->size; i++) {
    fread(&rec->fields[i].type, sizeof(DataType), 1, file);
    fread(&rec->fields[i].size, sizeof(size_t), 1, file);
    rec->fields[i].value = malloc(rec->fields[i].size);
    fread(rec->fields[i].value, rec->fields[i].size, 1, file);
  }
}