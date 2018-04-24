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
string _ENTRY;

/* Type checking and conversion */
bool check_op_type(node_attr * ss, node_attr * s1, node_attr * s2);
bool check_assign_type(node_attr * ss, node_attr * s1, node_attr * s2);
bool check_relop_type(node_attr * ss, node_attr * s1, node_attr * s2);
bool check_read_type(node_attr * ss, node_attr * s1);
bool check_write_type(node_attr * ss, node_attr * s1);
bool check_stmt_type(node_attr * ss, node_attr * s1, node_attr * s2);
bool check_complex_stmt_type(node_attr * ss, node_attr * s1, node_attr * s2, node_attr * s3);
bool check_stmt_seq_type(node_attr * ss, node_attr * s1, node_attr * s2);
bool check_integrity1(node_attr * ss, node_attr * s1);
bool check_integrity2(node_attr * ss, node_attr * s1, node_attr * s2);
bool check_integrity3(node_attr * ss, node_attr * s1, node_attr * s2, node_attr * s3);
bool check_integrity_stmt_seq(node_attr * ss, node_attr * s1, node_attr * s2);
#endif