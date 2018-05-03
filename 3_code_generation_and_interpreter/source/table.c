#include "../headers/table.h"
#include "../headers/typehandle.h"

int tempCounter = 0;

/* GLib Hash functions */

guint hash_func (gconstpointer key) {
    // Our hash function is literally the hash of the key string
    return g_str_hash((string) key);
}

gboolean key_equal_func (gconstpointer a, gconstpointer b) {
    string key_a = (string) a;
    string key_b = (string) b;

    if (strcmp(key_a, key_b) == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void key_destroy_func (gpointer data){
    free(data);
}

// Use this function only for entry_sym pointers stores in the symbol table
void value_destroy_fun (gpointer data){
    
    if (data != NULL) {
        sym_entry * entry = (sym_entry*) data;
        
        if (entry->identifier != NULL) {
            free(entry->identifier);
        }
        
        // We cannot free the type because it is a EXPLICIT STRING
        entry->type = NULL; 

        free(entry); //Check
    }
}


void print_hash_table (GHashTable * table){

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init (&iter, table);
    
    printf("\n+------------------------------------------------------+\n");
    printf("|                  Symbol Hash Table                   |\n");
    printf("+---------+------------+----------------+--------------+\n");
    printf("|   KEY   | identifier |      type      |     value    |\n");
    printf("+---------+------------+----------------+--------------+\n");

    // Iterate hash table to print every item
    while (g_hash_table_iter_next (&iter, &key, &value)) {

        if (value == NULL || key == NULL) {
            printf("|______________________________________________________|");
        } else {
            sym_entry * entry = (sym_entry*) value;
            printf("| %*.*s | %*.*s | %*.*s |",7,7,(string)key,10,10, entry->identifier,14,14, entry->type);
            
            if (entry->type == _INT) {
                printf(" %12d |\n",entry->value.ival);
            } else if (entry->type == _FLOAT) {
                printf(" %12.2f |\n",entry->value.fval);
            } else {
                printf(" _________ |\n");
            }
        }
    }

    printf("+---------+------------+----------------+--------------+\n");
}


sym_entry * create_temp_entry(string type){
    sym_entry * entry = (sym_entry *) malloc(sizeof(sym_entry));
    entry->identifier = "temp";
    entry->type = type;
    entry->value.ival = 0;
    
    return entry;
}

void print_sym_entry(sym_entry * symp){
    printf("%s %s\n",symp->identifier,symp->type);
}

/* Function to manage symbol table */

sym_entry * symput (string s) {

    // The table hasn't been created
    if (symtable == NULL) {
        printf("CRITICAL ERROR: The symbol table has not been created yet.");
        return NULL;
    }

    gpointer gp = s;
    
    // Search the key in the hash table
    if (g_hash_table_contains(symtable, gp) == TRUE) {

        return NULL;
    
    } else {
        // Create a new symbol entry
        sym_entry * entry = (sym_entry *) malloc(sizeof(sym_entry));
        entry->identifier = strdup(s);
        entry->type = _UNDEF;
        entry->value.ival = 0;

        // We duplicate s instead of using entry->identifier as a key to avoid double free
        if (g_hash_table_insert(symtable, strdup(s), entry) == TRUE) {
            return entry;
        } else {
            return NULL;
        }
    }   
}

sym_entry * symlook (string s) {

    // The table hasn't been created
    if (symtable == NULL) {
        printf("CRITICAL ERROR: The symbol table has not been created yet.");
        return NULL;
    }

    gpointer gp = s;
    
    // Search the key in the hash table
    if (g_hash_table_contains(symtable, gp) == TRUE) {

        return g_hash_table_lookup (symtable, gp);
    
    } else {
        
        return NULL;
    }   
}

sym_entry *new_temp(string type) {
     // The table hasn't been created
    if (symtable == NULL) {
        printf("CRITICAL ERROR: The symbol table has not been created yet.");
        return NULL;
    }


    // Generate variable identifier
    string s = gen_temp_id();
    gpointer gp = s;
    
    // Search the key in the hash table
    if (g_hash_table_contains(symtable, gp) == TRUE) {

        return NULL;
    
    } else {
        // Create a new symbol entry
        sym_entry * entry = (sym_entry *) malloc(sizeof(sym_entry));
        entry->identifier = s;
        entry->type = type;
        if (type == _FLOAT) {
            entry->value.fval = 0.0;
        } else {
            entry->value.ival = 0;
        }

        // We duplicate s instead of using entry->identifier as a key to avoid double free
        string key = strdup(s);
        if (g_hash_table_insert(symtable, key, entry) == TRUE) {
            return entry;
        } else {
            free(key);
            return NULL;
        }
    }   
}

string gen_temp_id() {
    string number = malloc(sizeof(char) * 12);
    sprintf(number, "t%d", tempCounter);
    tempCounter++;

    return number;
}