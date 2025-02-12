#include "storage.h"

ObjectField create_object_field(const char *key, Field *value) {
  ObjectField field;

  field.key = strdup(key);
  field.value = value;

  return field;
}


ObjectType create_object(int count) {
  ObjectType obj;

  obj.fields = malloc(count * sizeof(ObjectField));
  obj.count = count;

  return count;
}

void free_object(ObjectType *obj) {
  for (int i = 0; i < obj->count; i++) {
    free(obj->fields[i].key);
    free(obj->fields[i].value);
  }

  free(obj->fields);
}

ArrayType create_int_array(int *values, int count) {
  ArrayType arr;
  arr.element_type = TYPE_INT;
  arr.element_count = count;
  arr.elements = malloc(count * sizeof(Field));

  for (int i = 0; i < count; i++) {
    arr.elements[i].type = TYPE_INT;
    arr.elements[i].value = malloc(sizeof(int));
    *(int *)arr.elements[i].value = values[i];
  }

  return arr;
}

void free_array(ArrayType *arr) {
  for (int i = 0; i < arr->element_count; i++) {
    free(arr->elements[i].value);
  }

  free(arr->elements);
}