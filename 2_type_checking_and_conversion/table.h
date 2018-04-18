#ifndef TABLE_H
#define TABLE_H

#include "definitions.h"

GHashTable * symtable;

struct _sym_entry {
    string identifier;                    /* The name is just the string */
    string type;
    long value;                          /* The value is a float */
};

/*
union _v_value {
    int ival;
    float fval;
};

typedef struct _v_value v_value;
*/

typedef struct _sym_entry sym_entry;

/* Function prototype for the symbol table look up routine */
sym_entry * create_temp_entry(string type);
void print_sym_entry(sym_entry * symp);

guint hash_func (gconstpointer key);
gboolean key_equal_func (gconstpointer a, gconstpointer b);
void key_destroy_func (gpointer data);
void value_destroy_fun (gpointer data);
void print_hash_table (GHashTable * table);

sym_entry *symlook(string s);

#endif