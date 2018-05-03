#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "definitions.h"
#include "attribute.h"

/* General functions */
void semantic_warning(string message);
void semantic_error(string message);

bool check_variable(node_attr * ss, string identifier);
bool check_definition(node_attr * ss, string identifier, string type);

#endif