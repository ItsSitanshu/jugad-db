#ifndef SESSION_H
#define SESSION_H

#include "parser.h"

#include <stdbool.h>

#define _R   "\033[0m"
#define _BLD    "\033[1m"
#define _GRN   "\033[92m"  // Bright Green
#define _YLW  "\033[33m"
#define _RED     "\033[31m"
#define _BLU    "\033[38;2;176;224;230m"
#define _CY    "\033[36m"
#define _MAG "\033[1;35m"



void start_session();

bool execute_command(const char* db, SQLCommand *cmd);

#endif
