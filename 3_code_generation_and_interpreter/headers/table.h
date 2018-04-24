#ifndef TABLE_H
#define TABLE_H

#include "definitions.h"

GHashTable * symtable;
int tempCounter;

union _v_value {
    int ival;
    float fval;
};

typedef union _v_value v_value;

struct _sym_entry {
    string identifier;                    /* The name is just the string */
    string type;
    v_value value;                          /* The value is a float */
};

typedef struct _sym_entry sym_entry;

/* Function prototype for the symbol table look up routine */
sym_entry * create_temp_entry(string type);
void print_sym_entry(sym_entry * symp);

guint hash_func (gconstpointer key);
gboolean key_equal_func (gconstpointer a, gconstpointer b);
void key_destroy_func (gpointer data);
void value_destroy_fun (gpointer data);
void print_hash_table (GHashTable * table);

sym_entry *symput(string s);
sym_entry *symlook(string s);

sym_entry *new_temp(string type);
string gen_temp_id();

//void insert_entry (GHashTable * symtable, sym_entry * entry);

#endif