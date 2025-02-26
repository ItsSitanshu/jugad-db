#ifndef JDL_H
#define JDL_H

#include "storage.h"

void create_table(const char *db, const char *table_name, Column *columns, int column_count);

#endif // JDL_H
