#include "../headers/interpreter.h"

int program_index = 0;

void next_line() {
    program_index++;
}

void goto_line(int line){
    program_index = line;
}

void interpreter (GList * quad_list) {
    int length = g_list_length(quad_list);
    quad * quads[length];

    GList * temp = quad_list;

    do {
        quad * q = (quad *) temp->data;
        quads[q->line] = q;
    } while ( (temp = temp->next) != NULL );

    execute_program(length, quads);
}

void execute_program (int length, quad * quads[]) {
    while (program_index < length) {
        execute_quad(quads[program_index]);
    }
}

v_value try_casting(sym_entry * dest, sym_entry * src) {
    //v_value value = src->value;
    v_value casted = src->value; 

    if (dest->type != src->type) {
        if (dest->type == _INT) {
            int new = (int) src->value.fval;
            casted.ival = new;
        } else if (dest->type == _FLOAT) {
            float new = (float) src->value.ival;
            casted.fval = new;
        } else {
            printf("[CRITICAL ERROR]: Invalid value conversion\n");
        }
    }

    return casted;
}

void exec_sum(quad * q) {
    sym_entry * dest = q->f1.dest;
    sym_entry * src1 = q->f2.src;
    sym_entry * src2 = q->f3.src;

    v_value val1 = try_casting(dest, src1);
    v_value val2 = try_casting(dest, src2);

    if (dest->type == _INT) {
        dest->value.ival = val1.ival + val2.ival;
    } else if (dest->type == _FLOAT) {
        dest->value.fval = val1.fval + val2.fval;
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    next_line();
}

void exec_subs(quad * q) {
    sym_entry * dest = q->f1.dest;
    sym_entry * src1 = q->f2.src;
    sym_entry * src2 = q->f3.src;

    v_value val1 = try_casting(dest, src1);
    v_value val2 = try_casting(dest, src2);

    if (dest->type == _INT) {
        dest->value.ival = val1.ival - val2.ival;
    } else if (dest->type == _FLOAT) {
        dest->value.fval = val1.fval - val2.fval;
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    next_line();
}

void exec_mult(quad * q) {
    sym_entry * dest = q->f1.dest;
    sym_entry * src1 = q->f2.src;
    sym_entry * src2 = q->f3.src;

    v_value val1 = try_casting(dest, src1);
    v_value val2 = try_casting(dest, src2);

    if (dest->type == _INT) {
        dest->value.ival = val1.ival * val2.ival;
    } else if (dest->type == _FLOAT) {
        dest->value.fval = val1.fval * val2.fval;
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    next_line();
}

void exec_div(quad * q) {
    sym_entry * dest = q->f1.dest;
    sym_entry * src1 = q->f2.src;
    sym_entry * src2 = q->f3.src;

    v_value val1 = try_casting(dest, src1);
    v_value val2 = try_casting(dest, src2);

    if (dest->type == _INT) {
        dest->value.ival = val1.ival / val2.ival;
    } else if (dest->type == _FLOAT) {
        dest->value.fval = val1.fval / val2.fval;
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }

    next_line();
}

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
            v_value val1 = try_casting(dest, src);

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

void exec_jlt(quad * q){
    sym_entry * src1 = q->f1.src;
    sym_entry * src2 = q->f2.src;

    v_value val1 = src1->value;
    v_value val2 = src2->value;

    if (src1->type == _INT) {
        if (val1.ival < val2.ival) {
            goto_line(q->f3.address);
        } else {
            next_line();
        }
    } else if (src1->type == _FLOAT) {
        if (val1.fval < val2.fval) {
            goto_line(q->f3.address);
        } else {
            next_line();
        }
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }
}

void exec_je(quad * q){
    sym_entry * src1 = q->f1.src;
    sym_entry * src2 = q->f2.src;

    v_value val1 = src1->value;
    v_value val2 = src2->value;

    if (src1->type == _INT) {
        if (val1.ival == val2.ival) {
            goto_line(q->f3.address);
        } else {
            next_line();
        }
    } else if (src1->type == _FLOAT) {
        if (val1.fval == val2.fval) {
            goto_line(q->f3.address);
        } else {
            next_line();
        }
    } else {
        printf("[CRITICAL ERROR]: Unexpected type\n");
    }
}


void exec_jump(quad * q){
    goto_line(q->f3.address);
}

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

void execute_quad (quad * q) {
    //printf("Line: %d\n", program_index);
    switch(q->ins) {
        case ADDITION:
            exec_sum(q);
            break;
        case SUBTRACTION:
            exec_subs(q);
            break;
        case MULTIPLICATION:
            exec_mult(q);
            break;
        case DIVISION:
            exec_div(q);
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