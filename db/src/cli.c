#include <stdio.h>
#include "context.h"

int main(int argc, char* argv[]) {
  Context* ctx = ctx_init();
  if (!ctx) {
    fprintf(stderr, "Failed to initialize context\n");
    return 1;
  }

  if (argc > 1) {
    change_db(ctx, argv[1]);
  } else {
    fprintf(stderr, "No database file provided. Using default.\n");
    change_db(ctx, "default.db");
  }

  char* buffer = "CREATE TABLE table_name (column1, column2, column3);";

  ExecutionResult result = process(ctx, buffer);
  printf("Result: %s\n", result.message);

  ctx_free(ctx);
  return result.status_code;
}