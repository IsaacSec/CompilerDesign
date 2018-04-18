#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "definitions.h"

/* General functions */
void semantic_warning(string message);
void semantic_error(string message);

bool check_variable(sym_entry ** ss, string identifier);
bool check_definition(sym_entry ** ss, string identifier, string type);

#endif