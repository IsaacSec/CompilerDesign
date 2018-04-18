#include "typehandle.h"

string _FLOAT = "float";
string _INT = "integer";
string _UNDEF = "undefined";
string _BOOL = "boolean";
string _ERROR = "error";
string _EMPTY = "empty";

bool type_checking_op(sym_entry ** ss, sym_entry * s1, sym_entry * s2){
    sym_entry * sp = create_temp_entry(_UNDEF);
    *ss = sp;

    if (s1->type == s2->type) {
        sp->type = s1->type;
        return true;
    } else {
        // Type conversion
        if (s1->type == _INT && s2->type == _FLOAT) {
            sp->type = _FLOAT;
            syntantic_warning("conversion INT to FLOAT\n");
            // Value conversion
            return true;
        } else if (s1->type == _FLOAT && s2->type == _INT) {
            sp->type = _FLOAT;
            syntantic_warning("conversion INT to FLOAT\n");
            // Value conversion
            return true;
        } else {
            sp->type = _ERROR;
            syntantic_error("Cannot convert types\n");
            return false;
        }
    }
}

bool type_checking_relop(sym_entry ** ss, sym_entry * s1, sym_entry * s2){
    sym_entry * sp = create_temp_entry(_UNDEF);
    *ss = sp;

    if (s1->type == s2->type) {
        sp->type = _BOOL;
        return true;
    } else {
        // Type conversion 
        if (s1->type == _INT && s2->type == _FLOAT) {
            sp->type = _BOOL;
            syntantic_warning("conversion INT to FLOAT\n");
            // Value conversion
            return true;
        } else if (s1->type == _FLOAT && s2->type == _INT) {
            sp->type = _BOOL;
            syntantic_warning("conversion INT to FLOAT\n");
            // Value conversion
            return true;
        } else {
            sp->type = _ERROR;
            syntantic_error("Cannot convert types\n");
            return false;
        }
    }

    //ss->value=1;
}

bool type_checking_assign(sym_entry ** ss, sym_entry * s1, sym_entry * s2){
    sym_entry * sp = create_temp_entry(_UNDEF);
    *ss = sp;

    if (s1->type == s2->type) {
        sp->type = _EMPTY;
        return true;
    } else {
        // Type conversion 
        if (s1->type == _INT && s2->type == _FLOAT) {
            sp->type = _ERROR;
            // Value conversion
            syntantic_error("Cannot assign Float to INT\n");
            return false;
        } else if (s1->type == _FLOAT && s2->type == _INT) {
            sp->type = _EMPTY;
            // Value conversion
            return true;
        } else {
            sp->type = _ERROR;
            syntantic_error("Cannot convert ");
            printf("%s into %s\n",s1->type,s2->type);
            return false;
        }
    }
}