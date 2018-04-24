#include "../headers/codegen.h"

GList * new_list(quad * q) {
    GList * list = NULL;
    list = g_list_append(list, q);
    return list;
}

GList * merge(GList * a, GList * b) {
    return g_list_concat(a, b);
}

void backpatch (quad * q, int newLine) {
    
}


quad * gen_temp_constant_quad(string type, v_value constant) {
    sym_entry * temp = new_temp(type);
    v_value zero;
    q_type t;

    if (type == _INT){
        zero.ival = 0;
        t = Q_CONST_INTEGER; 
    } else if (type == _FLOAT) {
        zero.fval = 0.0;
        t = Q_CONST_FLOAT;
    }

    quad * q = create_constant_quad(next_quad(), t, temp, constant, zero);
    return q;
}

quad * gen_op_quad(node_attr * ss, node_attr * s1, node_attr * s2, instruction op) {

    sym_entry * temp = new_temp(ss->entry->type);
    sym_entry * e1 = s1->entry;
    sym_entry * e2 = s2->entry;

    quad * q = create_operation_quad(next_quad(), Q_OPERATION, op, temp, e1, e2);
    return q;
}

void gen_op_quad_list(node_attr * ss, node_attr * s1, node_attr * s2, instruction op) {
    quad * q = gen_op_quad(ss, s1, s2, op);
    ss->next_list = merge(s1->next_list, s2->next_list);
    ss->next_list = g_list_append(ss->next_list, q);
}