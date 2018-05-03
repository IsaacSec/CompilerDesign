#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "definitions.h"
#include "table.h"
#include "quad.h"

void interpreter (GList * quad_list);
void execute_program (int length, quad * quads[]);
void execute_quad (quad * q);

#endif