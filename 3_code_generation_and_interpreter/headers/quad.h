#ifndef QUAD_H
#define QUAD_H

#include "definitions.h"
#include "table.h"

enum _instruction {
    SUM,
    MULT,
    DIV,
    JUMP,
    JLT,
    JE
};

typedef enum _instruction instruction;

union _field {
    sym_entry * src;
    sym_entry * dest;
    int address;
};

typedef union _field field;

struct _quad {
    int line;
    instruction ins;
    field f1;
    field f2;
    field f3;
};

typedef struct _quad quad;

quad * create_operation_quad (  int line,
                                instruction ins, 
                                sym_entry * dest, 
                                sym_entry * src1, 
                                sym_entry * src2);

quad * create_procedure_quad (  int line,
                                instruction ins,
                                sym_entry * src1,
                                sym_entry * src2,
                                int address);

#endif