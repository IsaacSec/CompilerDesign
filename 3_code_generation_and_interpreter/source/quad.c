#include "../headers/quad.h"

int nextQuad = 1;

string S_ADD = "+";
string S_SUBTRACTION = "-";
string S_MULT = "*";
string S_DIV = "/";
string S_JUMP = "jump";
string S_JLT = "jlt";
string S_JE = "je";
string S_ASSIGN = ":=";

int next_quad() {
    int line = nextQuad;
    nextQuad ++;
    return line;   
}

quad * create_operation_quad (int line, q_type type, instruction ins, sym_entry * dest, sym_entry * src1, sym_entry * src2) {
    quad * q = (quad *) malloc(sizeof(quad));

    q->line = line;
    q->type = type;
    q->ins = ins;
    q->f1.dest = dest;
    q->f2.src = src1;
    q->f3.src = src2;

    return q;
}

quad * create_procedure_quad (int line, q_type type, instruction ins, sym_entry * src1, sym_entry * src2, int address) {
    quad * q = (quad *) malloc(sizeof(quad));

    q->line = line;
    q->type = type;
    q->ins = ins;
    q->f1.src = src1;
    q->f2.src = src2;
    q->f3.address = address;

    return q;
}

quad * create_constant_quad ( int line, q_type type, sym_entry * dest, v_value constant, v_value zero ) {
    quad * q = (quad *) malloc(sizeof(quad));

    q->line = line;
    q->type = type;
    q->ins = ASSIGN_TO;
    q->f1.dest = dest;
    q->f2.constant = constant;

    return q;
} 

string ins_to_string(instruction ins){
    switch(ins) {
        case ADDITION: return S_ADD;
        case SUBTRACTION: return S_SUBTRACTION;
        case MULTIPLICATION: return S_MULT;
        case DIVISION: return S_DIV;
        case JUMP: return S_JUMP;
        case JLT: return S_JLT;
        case JE: return S_JE;
        case ASSIGN_TO: return S_ASSIGN;
        default: return _ERROR;
    }
}

void print_quad(quad * q) {
    string instruction = ins_to_string(q->ins);
    switch(q->type) {
        case Q_CONST_INTEGER:
            printf("%s %s %d _\n", instruction, q->f1.dest->identifier, q->f2.constant.ival);
            break;
        case Q_CONST_FLOAT:
            printf("%s %s %f _\n", instruction, q->f1.dest->identifier, q->f2.constant.fval);
            break;
        case Q_OPERATION:
            printf("%s %s %s %s\n", instruction, q->f1.dest->identifier, q->f2.src->identifier, q->f3.src->identifier);
            break;
        case Q_JUMP:
            printf("%s %s %s %d\n", instruction, q->f1.src->identifier, q->f2.src->identifier, q->f3.address);
            break;
    }
}

void print_quad_in_list (gpointer data, gpointer user_data) {
    quad * q = (quad *) data;
    print_quad(q);
}

void print_quads(GList * quads) {
    g_list_foreach(quads, print_quad_in_list, NULL);
}