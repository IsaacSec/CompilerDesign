#include "../headers/typehandle.h"

string _FLOAT = "float";
string _INT = "integer";
string _UNDEF = "undefined";
string _BOOL = "boolean";
string _ERROR = "error";
string _EMPTY = "empty";
string _ENTRY = "entry";

/* These functions returns true if there is no error in type checking
* otherwise returns false
*/

bool check_op_type(node_attr * ss, node_attr * s1, node_attr * s2){
    
    sym_entry * entry;
    string type;
    bool check;
    
    if (check_integrity2(ss, s1, s2) == false) {
        ss->type = _ERROR;
        return false;
    }

    sym_entry * e1 = s1->entry;
    sym_entry * e2 = s2->entry;    

    if (e1->type == e2->type) {
        type = e1->type;
        check = true;
    } else {
        // Type conversion
        if (e1->type == _INT && e2->type == _FLOAT) {
            type = _FLOAT;
            check = true;
            semantic_warning("Conversion integer to float\n");
            // Value conversion
        } else if (e1->type == _FLOAT && e2->type == _INT) {
            type = _FLOAT;
            check = true;
            semantic_warning("Conversion integer to float\n");
            // Value conversion
        } else {
            ss->type = _ERROR;
            semantic_error("Cannot convert types\n");
            return false;
        }
    }

    entry = new_temp(type);
    ss->entry = entry;
    return check;
}

bool check_assign_type(node_attr * ss, node_attr * s1, node_attr * s2){

    if (check_integrity2(ss, s1, s2) == false){
        ss->type = _ERROR;
        return false;
    }

    if (s2->type == _BOOL) {
        ss->type = _ERROR;
        semantic_error("Cannot assign an expression to a variable\n");
        return false;
    }

    sym_entry * e1 = s1->entry;
    sym_entry * e2 = s2->entry;

    if (e1->type == e2->type) {
        ss->type = _EMPTY;
        return true;
    } else {
        // Type conversion
        if (e1->type == _INT && e2->type == _FLOAT) {
            ss->type = _ERROR;
            // Value conversion
            semantic_error("Cannot assign float to integer variable\n");
            return false;
        } else if (e1->type == _FLOAT && e2->type == _INT) {
            ss->type = _EMPTY;
            // Value conversion
            return true;
        } else {
            ss->type = _ERROR;
            semantic_error("Cannot assign ");
            printf("%s to %s variable\n",e2->type,e1->type);
            return false;
        }
    }
}

bool check_relop_type(node_attr * ss, node_attr * s1, node_attr * s2){

    if (check_integrity2(ss, s1, s2) == false){
        ss->type = _ERROR;
        return false;
    }

    sym_entry * e1 = s1->entry;
    sym_entry * e2 = s2->entry;

    if (e1->type == e2->type) {
        ss->type = _BOOL;
        return true;
    } else {
        // Type conversion 
        if (e1->type == _INT && e2->type == _FLOAT) {
            ss->type = _BOOL;
            semantic_warning("Conversion interger to float\n");
            // Value conversion
            return true;
        } else if (e1->type == _FLOAT && e2->type == _INT) {
            ss->type = _BOOL;
            semantic_warning("Conversion integer to float\n");
            // Value conversion
            return true;
        } else {
            ss->type = _ERROR;
            semantic_error("Cannot convert types\n");
            return false;
        }
    }
}

bool check_read_type(node_attr * ss, node_attr * s1) {
    
    if (check_integrity1(ss, s1) == false) {
        ss->type = _ERROR;
        return false;
    }

    sym_entry * e = s1->entry;

    if (e->type == _FLOAT || e->type == _INT) {
        ss->type = _EMPTY;
        return true;
    } else {
        ss->type = _ERROR;
        semantic_error("The variable must be an integer or float type\n");
        return false;
    }
}

bool check_write_type(node_attr * ss, node_attr * s1) {

    if (check_integrity1(ss, s1) == false) {
        ss->type = _ERROR;
        return false;
    }

    sym_entry * e = s1->entry;

    if (e->type == _FLOAT || e->type == _INT) {
        return true;
    } else {
        ss->type = _ERROR;
        semantic_error("The expression must be an integer or a float type\n");
        return false;
    }
}

bool check_stmt_type(node_attr * ss, node_attr * s1, node_attr * s2) {

    if (check_integrity2(ss, s1, s2) == false){
        ss->type = _ERROR;
        return false;
    }

    if (s1->type == _BOOL && s2->type == _EMPTY){
        return true;
    } else {
        ss->type = _ERROR;
        semantic_error("Invalid expression in statement\n");
        return false; 
    }
}

bool check_complex_stmt_type(node_attr * ss, node_attr * s1, node_attr * s2, node_attr * s3) {

    if (check_integrity3(ss, s1, s2, s3) == false) {
        ss->type = _ERROR;
        return false;
    }
    
    if (s1->type == _BOOL){
        if (s2->type == _EMPTY && s3->type == _EMPTY){
            return true;
        } else {
            ss->type = _ERROR;
            return false;
        }
    } else {
        semantic_error("Invalid expression in statement\n");
        ss->type = _ERROR;
        return false;
    }
}


bool check_stmt_seq_type(node_attr * ss, node_attr * s1, node_attr * s2) {

    if (check_integrity_stmt_seq(ss, s1, s2) == false){
        ss->type = _ERROR;
        return false;
    }
    
    if (s2->type == _EMPTY){
        return true;
    } else {
        ss->type = _ERROR;
        return false;
    }
}

// --------------------------------------------------------------
/* The check_integrity functions tries to identify the must common error sources
 * such as Error types and NULL pointers
 */
bool check_integrity1(node_attr * ss, node_attr * s1){
    if (ss == NULL || s1 == NULL ){
        printf("[CRITICAL ERROR: Null attribute]\n");
        return false;
    }

    if (s1->type == _ERROR ){
        return false;
    }

    if (s1->type == _ENTRY && s1->entry == NULL) {
        printf("[CRITICAL ERROR: Null entry]\n");
        return false;
    }

    return true;
}

bool check_integrity2(node_attr * ss, node_attr * s1, node_attr * s2){
    if (ss == NULL || s1 == NULL || s2 == NULL){
        printf("[CRITICAL ERROR: Null attribute]\n");
        return false;
    }

    if (s1->type == _ERROR || s2->type == _ERROR){
        //printf("[Error]: Nested error\n");
        return false;
    }

    if (s1->type == _ENTRY && s1->entry == NULL) {
        printf("[CRITICAL ERROR: Null entry]\n");
        return false;
    }

    if (s2->type == _ENTRY && s2->entry == NULL) {
        printf("[CRITICAL ERROR: Null entry]\n");
        return false;
    }

    return true;
}

bool check_integrity3(node_attr * ss, node_attr * s1, node_attr * s2, node_attr * s3){
    if (ss == NULL || s1 == NULL || s2 == NULL || s3 == NULL){
        printf("[CRITICAL ERROR: Null attribute]\n");
        return false;
    }

    if (s1->type == _ERROR || s2->type == _ERROR || s3->type == _ERROR){
        return false;
    }

    if (s1->type == _ENTRY && s1->entry == NULL) {
        printf("[CRITICAL ERROR: Null entry]\n");
        return false;
    }

    if (s2->type == _ENTRY && s2->entry == NULL) {
        printf("[CRITICAL ERROR: Null entry]\n");
        return false;
    }

    if (s3->type == _ENTRY && s3->entry == NULL) {
        printf("[CRITICAL ERROR: Null entry]\n");
        return false;
    }

    return true;
}

bool check_integrity_stmt_seq(node_attr * ss, node_attr * s1, node_attr * s2){
    if (ss == NULL || s1 == NULL || s2 == NULL){
        printf("[CRITICAL ERROR: Null attribute]\n");
        return false;
    }

    if (s2->type == _ERROR){
        //printf("[Error]: Nested error\n");
        return false;
    }

    if (s1->type == _ENTRY && s1->entry == NULL) {
        printf("[CRITICAL ERROR: Null entry]\n");
        return false;
    }

    if (s2->type == _ENTRY && s2->entry == NULL) {
        printf("[CRITICAL ERROR: Null entry]\n");
        return false;
    }

    return true;
}