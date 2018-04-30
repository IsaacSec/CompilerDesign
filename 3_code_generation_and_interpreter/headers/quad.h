#ifndef QUAD_H
#define QUAD_H

#include "definitions.h"
#include "table.h"
#include "typehandle.h"

int nextQuad;

enum _instruction {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    JUMP,
    JLT,
    JE,
    ASSIGN_TO,
    READ_VAR,
    WRITE_EXP
};

typedef enum _instruction instruction;

union _field {
    sym_entry * src;
    sym_entry * dest;
    v_value constant;
    int address;
};

typedef union _field field;

enum _q_type{
    Q_CONST_INTEGER,
    Q_CONST_FLOAT,
    Q_OPERATION,
    Q_JUMP,
    Q_FUNCTION
};

typedef enum _q_type q_type;

struct _quad {
    int line;
    q_type type;
    instruction ins;
    field f1;
    field f2;
    field f3;
};

typedef struct _quad quad;

int next_quad();
int get_next_quad();
void remove_quads(int toRemove);

quad * create_operation_quad (  int line,
                                q_type type,
                                instruction ins, 
                                sym_entry * dest, 
                                sym_entry * src1, 
                                sym_entry * src2);

quad * create_procedure_quad (  int line,
                                q_type type,
                                instruction ins,
                                sym_entry * src1,
                                sym_entry * src2,
                                int address);

quad * create_constant_quad ( int line, q_type type, sym_entry * dest, v_value constant, v_value zero );

quad * create_function_quad (   int line,
                                q_type type,
                                instruction ins,
                                sym_entry * src);

string ins_to_string(instruction ins);
void print_quad(quad * q);
void print_quad_in_list (gpointer data, gpointer user_data);
void print_quads(GList * quads);

void free_quad_list_full(GList * quad_list);

#endif