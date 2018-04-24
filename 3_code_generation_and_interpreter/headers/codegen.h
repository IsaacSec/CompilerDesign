#ifndef CODE_GEN_H
#define CODE_GEN_H

#include "definitions.h"
#include "table.h"
#include "quad.h"
#include "attribute.h"
#include "typehandle.h"

// Be care with first argument (What should we use? List or a single quad)
GList * new_list(quad * q);
GList * merge(GList * a, GList * b);
void backpatch (quad * q, int newLine); 

quad * gen_temp_constant_quad(string type, v_value constant);
quad * gen_op_quad(node_attr * ss, node_attr * s1, node_attr * s2, instruction op);

void gen_op_quad_list(node_attr * ss, node_attr * s1, node_attr * s2, instruction op);

#endif