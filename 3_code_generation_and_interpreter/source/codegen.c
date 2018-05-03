#include "../headers/codegen.h"

// Creates a new list with a quad
GList * new_list(quad * q) {
    GList * list = NULL;
    list = g_list_append(list, q);
    return list;
}

// Concatenates two lists
GList * merge(GList * a, GList * b) {
    return g_list_concat(a, b);
}

// Backpatch a quad
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

// Backpatch a list of quads
void backpatch (GList * list, int newLine) {
    gpointer line = &newLine;
    g_list_foreach(list, backpatch_quad, line);
}

// Generates a quad for a constant
quad * gen_temp_constant_quad(string type, v_value constant) {
    sym_entry * temp = new_temp(type);
    q_type t;

    if (type == _INT){
        t = Q_CONST_INTEGER; 
    } else if (type == _FLOAT) {
        t = Q_CONST_FLOAT;
    } else {
        // Avoid warning (It displays a CRITICAL ERROR if it tried to use)
        t = 99;
    }

    temp->value = constant;
    quad * q = create_constant_quad(next_quad(), t, temp, constant);
    return q;
}

// Generates an operation quad
quad * gen_op_quad(node_attr * ss, node_attr * s1, node_attr * s2, instruction op) {

    if (op == ASSIGN_TO) {
        sym_entry * e1 = s1->entry;
        sym_entry * e2 = s2->entry;
        quad * q = create_operation_quad(next_quad(), Q_OPERATION, op, e1, e2, NULL);
        return q;
    } else {
        sym_entry * e1 = s1->entry;
        sym_entry * e2 = s2->entry;

        quad * q = create_operation_quad(next_quad(), Q_OPERATION, op, ss->entry, e1, e2);
        return q;
    }
}


// Generates a list of quads resultant from an operation
void gen_op_quad_list(node_attr * ss, node_attr * s1, node_attr * s2, instruction op) {
    quad * q = gen_op_quad(ss, s1, s2, op);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = merge(ss->quad_list, s2->quad_list);
    ss->quad_list = g_list_append(ss->quad_list, q);
}

// Generates a list of quads resultant from an relational operation
void gen_relop_quad_list(node_attr * ss, node_attr * s1, node_attr * s2, instruction ins) {
    sym_entry * e1 = s1->entry;
    sym_entry * e2 = s2->entry;

    quad * q1 = create_procedure_quad(next_quad(), Q_JUMP, ins, e1, e2, 0);
    quad * q2 = create_procedure_quad(next_quad(), Q_JUMP, JUMP, NULL, NULL, 0);

    ss->true_list = g_list_append(ss->true_list, q1);
    ss->false_list = g_list_append(ss->false_list, q2);

    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = merge(ss->quad_list, s2->quad_list);

    ss->quad_list = g_list_append(ss->quad_list, q1);
    ss->quad_list = g_list_append(ss->quad_list, q2);
}

// Generates a list of quads resultant from an IF THEN ELSE statement
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

// Generates a list of quads resultant from an IF THEN statement
void gen_if_then_quad_list(node_attr * ss, node_attr * e, int m1, node_attr * s1) {
    backpatch(e->true_list, m1);
    ss->next_list = merge(e->false_list, s1->next_list);

    ss->quad_list = merge(ss->quad_list, e->quad_list);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
}

// Generates a list of quads resultant from a WHILE statement
void gen_while_quad_list(node_attr * ss, int m1, node_attr * e, int m2, node_attr * s1) {
    backpatch(e->true_list, m2);
    backpatch(s1->next_list, m1);
    ss->next_list = merge(ss->next_list, e->false_list);

    quad * q = create_procedure_quad(next_quad(), Q_JUMP, JUMP, NULL, NULL, m1);
    ss->quad_list = merge(ss->quad_list, e->quad_list);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = g_list_append(ss->quad_list, q);
    
}

// Generates a list of quads resultant from a READ function
void gen_read_quad_list(node_attr * ss, node_attr * s1) {
    sym_entry * var = s1->entry;
    quad * q = create_function_quad(next_quad(), Q_FUNCTION, READ_VAR, var);
    ss->quad_list = g_list_append(ss->quad_list, q);
}

// Generates a list of quads resultant from a WRITE function
void gen_write_quad_list(node_attr * ss, node_attr * s1) {
    sym_entry * e = s1->entry;
    quad * q = create_function_quad(next_quad(), Q_FUNCTION, WRITE_EXP, e);
    ss->quad_list = merge(ss->quad_list, s1->quad_list);
    ss->quad_list = g_list_append(ss->quad_list, q);
}

// Suppress quad lists from given node_attributes 
void remove_error_attr_lines(int numberOfAttr, ...){
    int totalOfQuads = 0;
    node_attr * attr = NULL;
    va_list ap;

    printf("+------------------------------------------------------+\n");
    printf("|                    SUPRESSED CODE                    |\n");
    printf("+------------------------------------------------------+\n");

    va_start(ap, numberOfAttr);
        for (int i = 0; i < numberOfAttr; i++) {
            attr = va_arg(ap, node_attr *); // Get next argument
            print_quads(attr->quad_list);
            totalOfQuads += g_list_length(attr->quad_list);
            free_quad_list_full(attr->quad_list);
        }
    va_end(ap);

    remove_quads(totalOfQuads);
    printf("+------------------------------------------------------+\n\n");
}