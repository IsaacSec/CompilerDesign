#include "../headers/quad.h"

int nextQuad = 0;

string S_ADD = "+";
string S_SUBTRACTION = "-";
string S_MULT = "*";
string S_DIV = "/";
string S_JUMP = "jump";
string S_JLT = "jlt";
string S_JE = "je";
string S_ASSIGN = ":=";
string S_READ = "read";
string S_WRITE = "write";

int next_quad() {
    int line = nextQuad;
    nextQuad ++;
    return line;   
}

int get_next_quad() {
    return nextQuad;
}

void remove_quads(int toRemove) {
    nextQuad -= toRemove;
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

quad * create_function_quad ( int line, q_type type, instruction ins, sym_entry * src) {
    quad * q = (quad *) malloc(sizeof(quad));

    q->line = line;
    q->type = type;
    q->ins = ins;
    q->f1.src = src;

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
        case READ_VAR: return S_READ;
        case WRITE_EXP: return S_WRITE;
        default: return _ERROR;
    }
}

void print_quad(quad * q) {
    string instruction = ins_to_string(q->ins);
    int line = q->line;
    switch(q->type) {
        case Q_CONST_INTEGER:
            printf("|  [%3d] | %8s | %8s | %12d | ______ |\n", line, instruction, q->f1.dest->identifier, q->f2.constant.ival);
            break;
        case Q_CONST_FLOAT:
            printf("|  [%3d] | %8s | %8s | %12.3f | ______ |\n", line, instruction, q->f1.dest->identifier, q->f2.constant.fval);
            break;
        case Q_OPERATION:
            switch (q->ins) {
                case ASSIGN_TO:
                    printf("|  [%3d] | %8s | %8s | %12s | ______ |\n", line, instruction, q->f1.dest->identifier, q->f2.src->identifier);
                    break;
                default:
                    printf("|  [%3d] | %8s | %8s | %12s | %6s |\n", line, instruction, q->f1.dest->identifier, q->f2.src->identifier, q->f3.src->identifier);
            }
            break;
        case Q_JUMP:
            switch(q->ins){
                case JLT:
                    printf("|  [%3d] | %8s | %8s | %12s |  [%3d] |\n", line, instruction, q->f1.src->identifier, q->f2.src->identifier, q->f3.address);
                    break;
                case JE:
                    printf("|  [%3d] | %8s | %8s | %12s |  [%3d] |\n", line, instruction, q->f1.src->identifier, q->f2.src->identifier, q->f3.address);
                    break;
                case JUMP:
                    printf("|  [%3d] | %8s | ________ | ____________ |  [%3d] |\n", line, instruction, q->f3.address);
                    break;
                default: 
                    printf("[CRITICAL ERROR]: Unexpected instruction\n");
            }
            break;
        case Q_FUNCTION:
            switch(q->ins) {
                case READ_VAR:
                    printf("|  [%3d] | %8s | %8s | ____________ | ______ |\n", line, instruction, q->f1.src->identifier);
                    break;
                case WRITE_EXP:
                    printf("|  [%3d] | %8s | %8s | ____________ | ______ |\n", line, instruction, q->f1.src->identifier);
                    break;
                default:
                    printf("[CRITICAL ERROR]: Unexpected instruction\n");
            }
    }
}

void print_quad_in_list (gpointer data, gpointer user_data) {
    quad * q = (quad *) data;
    print_quad(q);
}

void print_quads(GList * quads) {
    g_list_foreach(quads, print_quad_in_list, NULL);
}