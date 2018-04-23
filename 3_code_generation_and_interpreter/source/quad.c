#include "../headers/quad.h"


quad * create_operation_quad (int line, instruction ins, sym_entry * dest, sym_entry * src1, sym_entry * src2) {
    quad * q = (quad *) malloc(sizeof(quad));

    q->line = line;
    q->ins = ins;
    q->f1.dest = dest;
    q->f2.src = src1;
    q->f3.src = src2;

    return q;
}

quad * create_procedure_quad (int line, instruction ins, sym_entry * src1, sym_entry * src2, int address) {
    quad * q = (quad *) malloc(sizeof(quad));

    q->line = line;
    q->ins = ins;
    q->f1.src = src1;
    q->f2.src = src2;
    q->f3.address = address;

    return q;
}