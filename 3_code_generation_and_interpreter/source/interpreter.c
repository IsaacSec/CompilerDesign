#include "../headers/interpreter.h"

int program_index = 0;

// Return the number of line in the next quad 
void next_line() {
    program_index++;
}

// Change the number of the current line, for control flow
void goto_line(int line){
    program_index = line;
}

// Executes the interpreter given a quad list
void interpreter (GList * quad_list) {

    if (quad_list == NULL) {
        return;
    }

    int length = g_list_length(quad_list);
    quad * quads[length];

    GList * temp = quad_list;

    // The quad list is passed to an array to reduce access time to each quad
    do {
        quad * q = (quad *) temp->data;
        quads[q->line] = q;
    } while ( (temp = temp->next) != NULL );

    // Execute quads
    execute_program(length, quads);
    free_quad_list_full(quad_list);
}

// Run the quad array based on the program index (this indicates the quad to be executed)
void execute_program (int length, quad * quads[]) {
    while (program_index < length) {
        execute_quad(quads[program_index]);
    }
}

// Tries to cast the value of an entry based on dest type
v_value try_casting(string dest_type, sym_entry * src) {
    v_value casted = src->value; 

    if (dest_type != src->type) {

        if (dest_type == _INT) {
            casted.ival = (int) src->value.fval;
        } else if (dest_type == _FLOAT) {
            casted.fval = (float) src->value.ival;
        } else {
            printf("[CRITICAL ERROR]: Invalid value conversion\n");
        }
    }

    return casted;
}

// Returns the prefered type between two types
string get_dominant_type(string t1, string t2) {
    string type = _ERROR;

    if (t1 != t2) {
        if (t1 == _INT && t2 == _FLOAT) {
            return _FLOAT;
        } else if (t1 == _FLOAT && t2 == _INT) {
            return _FLOAT;
        } else {
            printf("[CRITICAL ERROR]: Invalid value conversion\n");
        }
    } else {
        type = t1;
    }

    return type;
}

// Returns the sum between two values
v_value sum (string type, v_value a, v_value b) {
    v_value result = {.ival = 0};

    if (type == _INT) {
        result.ival = a.ival + b.ival;
    } else if (type == _FLOAT) {
        result.fval = a.fval + b.fval;
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    return result;
}

// Returns the substraction between two values
v_value subs (string type, v_value a, v_value b) {
    v_value result = {.ival = 0};

    if (type == _INT) {
        result.ival = a.ival - b.ival;
    } else if (type == _FLOAT) {
        result.fval = a.fval - b.fval;
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    return result;
}

// Returns the multiplication between two values
v_value mult (string type, v_value a, v_value b) {
    v_value result = {.ival = 0};

    if (type == _INT) {
        result.ival = a.ival * b.ival;
    } else if (type == _FLOAT) {
        result.fval = a.fval * b.fval;
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    return result;
}

// Returns the division between two values
v_value division (string type, v_value a, v_value b) {
    v_value result = {.ival = 0};

    if (type == _INT) {
        result.ival = a.ival / b.ival;
    } else if (type == _FLOAT) {
        result.fval = a.fval / b.fval;
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    return result;
}


// Executes an operation quad (SUM, MULT, DIV, SUBS)
void exec_op (quad * q, v_value (*op)(string, v_value, v_value)) {
    sym_entry * dest = q->f1.dest;
    sym_entry * src1 = q->f2.src;
    sym_entry * src2 = q->f3.src;

    v_value val1 = try_casting(dest->type, src1);
    v_value val2 = try_casting(dest->type, src2);

    dest->value = op(dest->type, val1, val2);

    next_line();
}

// Executes an assignment quad (:=)
void exec_assign(quad * q) {
    sym_entry * dest = q->f1.dest;
    sym_entry * src;

    switch (q->type) {
        case Q_CONST_INTEGER:
            dest->value.ival = q->f2.constant.ival;
            break;
        case Q_CONST_FLOAT:
            dest->value.fval = q->f2.constant.fval;
            break;
        case Q_OPERATION:
            src = q->f2.src;
            v_value val1 = try_casting(dest->type, src);

            if (dest->type == _INT) {
                dest->value.ival = val1.ival;
            } else if (dest->type == _FLOAT) {
                dest->value.fval = val1.fval;
            } else {
                printf("[CRITICAL ERROR]: Unexpected type\n");
            }
            break;
        default:
            printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    next_line();
}

// Executes a JUMP LESS THAN quad
void exec_jlt(quad * q){
    sym_entry * src1 = q->f1.src;
    sym_entry * src2 = q->f2.src;

    string type = get_dominant_type(src1->type, src2->type);
    // Guarantees that the two value have the same type
    v_value val1 = try_casting(type, src1);
    v_value val2 = try_casting(type, src2);

    if (type == _INT) {
        if (val1.ival < val2.ival) {
            goto_line(q->f3.address);
        } else {
            next_line();
        }
    } else if (type == _FLOAT) {
        if (val1.fval < val2.fval) {
            goto_line(q->f3.address);
        } else {
            next_line();
        }
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }
}

// Executes a JUMP EQUAL quad
void exec_je(quad * q){
    sym_entry * src1 = q->f1.src;
    sym_entry * src2 = q->f2.src;

    string type = get_dominant_type(src1->type, src2->type);
    // Guarantees that the two value have the same type
    v_value val1 = try_casting(type, src1);
    v_value val2 = try_casting(type, src2);

    if (type == _INT) {
        if (val1.ival == val2.ival) {
            goto_line(q->f3.address);
        } else {
            next_line();
        }
    } else if (type == _FLOAT) {
        if (val1.fval == val2.fval) {
            goto_line(q->f3.address);
        } else {
            next_line();
        }
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }
}

// Executes a JUMP quad
void exec_jump(quad * q){
    goto_line(q->f3.address);
}

// Executes a WRITE quad
void exec_write(quad * q){
    sym_entry * src = q->f1.src;
    v_value val = src->value;

    printf("[Output %10s]: ",src->identifier);
    if (src->type == _INT) {
        printf("%d\n", val.ival);
    } else if (src->type == _FLOAT) {
        printf("%f\n", val.fval);
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    next_line();
}

// Executes a READ quad
void exec_read(quad * q){
    sym_entry * src = q->f1.src;

    printf("[Input  %10s]: ",src->identifier);
    if (src->type == _INT) {
        scanf("%d",&src->value.ival);
    } else if (src->type == _FLOAT) {
        scanf("%f",&src->value.fval);
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    next_line();
}

// Execute a quad depending on the instruction
void execute_quad (quad * q) {
    switch(q->ins) {
        case ADDITION:
            exec_op(q, sum);
            break;
        case SUBTRACTION:
            exec_op(q, subs);
            break;
        case MULTIPLICATION:
            exec_op(q, mult);
            break;
        case DIVISION:
            exec_op(q, division);
            break;
        case JUMP:
            exec_jump(q);
            break;
        case JLT:
            exec_jlt(q);
            break;
        case JE:
            exec_je(q);
            break;
        case ASSIGN_TO:
            exec_assign(q);
            break;
        case READ_VAR:
            exec_read(q);
            break;
        case WRITE_EXP:
            exec_write(q);
            break;
        default:
            printf("[CRITICAL ERROR]: Unexpected instruction\n");
    }
}