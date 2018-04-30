#include "../headers/attribute.h"

node_attr * create_node_attr(string type) {
    node_attr * newNode = (node_attr*) malloc (sizeof (node_attr));
    newNode->type = type;
    newNode->true_list = NULL;
    newNode->false_list = NULL;
    newNode->next_list = NULL;
    newNode->quad_list = NULL;
    return newNode;
}

void print_attr(node_attr * n){
    printf("Type: %s ",n->type);
    if (n->entry != NULL){
        printf("sType: %s id: %s",n->entry->type, n->entry->identifier);
    }
    printf("\n");
}

void free_attr(node_attr * attr){
    free(attr);
}

void free_attrs(int numberOfAttr, ...){
    node_attr * attr = NULL;
    
    va_list ap;

    va_start(ap, numberOfAttr); /* Requires the last fixed parameter (to get the address) */
    for (int i = 0; i < numberOfAttr; i++) {
        attr = va_arg(ap, node_attr *); /* Increments ap to the next argument. */
        free_attr(attr);
    }
    va_end(ap);
}