#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "table.h"
#include "definitions.h"

struct _node_attr {
    sym_entry * entry;
    GList * false_list;
    GList * true_list;
    GList * next_list;
    GList * quad_list;
    int quad;
    string type;
};

typedef struct _node_attr node_attr;

node_attr * create_node_attr(string type);

void print_attr(node_attr * n);

#endif