#ifndef TYPE_HANDLE_H
#define TYPE_HANDLE_H

#include "definitions.h"
#include "table.h"
#include "semantic.h"

/* TYPES */
string _FLOAT;
string _INT;
string _UNDEF;
string _BOOL;
string _ERROR;
string _EMPTY;

/* Type checking and conversion */
bool check_op_type(sym_entry ** ss, sym_entry * s1, sym_entry * s2);
bool check_assign_type(sym_entry ** ss, sym_entry * s1, sym_entry * s2);
bool check_relop_type(sym_entry ** ss, sym_entry * s1, sym_entry * s2);


#endif