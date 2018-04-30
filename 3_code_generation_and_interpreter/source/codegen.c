#include "../headers/codegen.h"

GList * new_list(quad * q) {
    GList * list = NULL;
    list = g_list_append(list, q);
    return list;
}

GList * merge(GList * a, GList * b) {
    return g_list_concat(a, b);
}

void backpatch_quad (gpointer data, gpointer user_data) {
    int * newLine = (int *) user_data;
    quad * q = (quad *) data;

    if (q->type == Q_JUMP) {
        q->f3.address = *newLine;
    } else {
        printf ("[WARNING]: Backpatch in a not JUMP quad: ");
        print_quad(q);
    }
}

void backpatch (GList * list, int newLine) {
    gpointer line = &newLine;
    g_list_foreach(list, backpatch_quad, line);
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

    temp->value = constant;
    quad * q = create_constant_quad(next_quad(), t, temp, constant, zero);
    return q;
}

quad * gen_op_quad(node_attr * ss, node_attr * s1, node_attr * s2, instruction op) {

    if (op == ASSIGN_TO) {
        sym_entry * e1 = s1->entry;
        sym_entry * e2 = s2->entry;
        quad * q = create_operation_quad(next_quad(), Q_OPERATION, op, e1, e2, NULL);
        return q;
    } else {
        //sym_entry * temp = new_temp(ss->entry->type);
        //printf("Operation: %s\n", ss->entry->identifier);
        sym_entry * e1 = s1->entry;
        sym_entry * e2 = s2->entry;

        quad * q = create_operation_quad(next_quad(), Q_OPERATION, op, ss->entry, e1, e2);
        return q;
    }
}



void gen_op_quad_list(node_attr * ss, node_attr * s1, node_attr * s2, instruction op) {
    quad * q = gen_op_quad(ss, s1, s2, op);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = merge(ss->quad_list, s2->quad_list);
    ss->quad_list = g_list_append(ss->quad_list, q);
}

void gen_relop_quad_list(node_attr * ss, node_attr * s1, node_attr * s2, instruction ins) {
    sym_entry * e1 = s1->entry;
    sym_entry * e2 = s2->entry;

    // next_quad + 1 ???

    quad * q1 = create_procedure_quad(next_quad(), Q_JUMP, ins, e1, e2, 0);
    quad * q2 = create_procedure_quad(next_quad(), Q_JUMP, JUMP, NULL, NULL, 0);

    ss->true_list = g_list_append(ss->true_list, q1);
    ss->false_list = g_list_append(ss->false_list, q2);

    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = merge(ss->quad_list, s2->quad_list);

    ss->quad_list = g_list_append(ss->quad_list, q1);
    ss->quad_list = g_list_append(ss->quad_list, q2);
}

void gen_if_then_else_quad_list(node_attr * ss, node_attr * e, int m1, node_attr * s1, node_attr * n, int m2, node_attr * s2) {
    
    backpatch(e->true_list, m1);
    backpatch(e->false_list, m2);
    
    ss->next_list = merge(ss->next_list, s1->next_list);
    ss->next_list = merge(ss->next_list, n->next_list);
    ss->next_list = merge(ss->next_list, s2->next_list);

    ss->quad_list = merge(ss->quad_list, e->quad_list);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = merge(ss->quad_list, n->quad_list);
    ss->quad_list = merge(ss->quad_list, s2->quad_list);
}

void gen_if_then_quad_list(node_attr * ss, node_attr * e, int m1, node_attr * s1) {
    backpatch(e->true_list, m1);
    ss->next_list = merge(e->false_list, s1->next_list);

    ss->quad_list = merge(ss->quad_list, e->quad_list);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
}

void gen_while_quad_list(node_attr * ss, int m1, node_attr * e, int m2, node_attr * s1) {
    backpatch(e->true_list, m2);
    ss->next_list = merge(ss->next_list, e->false_list);

    quad * q = create_procedure_quad(next_quad(), Q_JUMP, JUMP, NULL, NULL, m1);
    ss->quad_list = merge(ss->quad_list, e->quad_list);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = g_list_append(ss->quad_list, q);
    
}

void gen_read_quad_list(node_attr * ss, node_attr * s1) {
    sym_entry * var = s1->entry;
    quad * q = create_function_quad(next_quad(), Q_FUNCTION, READ_VAR, var);
    ss->quad_list = g_list_append(ss->quad_list, q);
}

void gen_write_quad_list(node_attr * ss, node_attr * s1) {
    sym_entry * e = s1->entry;
    quad * q = create_function_quad(next_quad(), Q_FUNCTION, WRITE_EXP, e);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = g_list_append(ss->quad_list, q);
}

void remove_error_attr_lines(int numberOfAttr, ...){
    int totalOfQuads = 0;
    node_attr * attr = NULL;
    
    printf("+------------------------------------------------------+\n");
    printf("|                    SUPRESSED CODE                    |\n");
    printf("+------------------------------------------------------+\n");
    va_list ap;

    va_start(ap, numberOfAttr); /* Requires the last fixed parameter (to get the address) */
    for (int i = 0; i < numberOfAttr; i++) {
        attr = va_arg(ap, node_attr *); /* Increments ap to the next argument. */
        print_quads(attr->quad_list);
        totalOfQuads += g_list_length(attr->quad_list);
        free_quad_list_full(attr->quad_list);
    }
    va_end(ap);
    remove_quads(totalOfQuads);
    printf("+------------------------------------------------------+\n\n");
}