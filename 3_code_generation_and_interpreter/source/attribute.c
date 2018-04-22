#include "../headers/attribute.h"

node_attr * create_node_attr(string type) {
    node_attr * newNode = (node_attr*) malloc (sizeof (node_attr));
    newNode->type = type;
    return newNode;
}

void print_attr(node_attr * n){
    printf("Type: %s ",n->type);
    if (n->entry != NULL){
        printf("sType: %s id: %s",n->entry->type, n->entry->identifier);
    }
    printf("\n");
}