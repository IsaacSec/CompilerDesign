#include <stdbool.h>
typedef char * string;

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

void syntantic_warning(string message);
void syntantic_error(string message);

/* Function prototype for the symbol table look up routine */
sym_entry *symlook(string s);
sym_entry * create_temp_entry(string type);

/* Type checking and conversion */
bool type_checking_op(sym_entry ** ss, sym_entry * s1, sym_entry * s2);
bool type_checking_assign(sym_entry ** ss, sym_entry * s1, sym_entry * s2);
bool type_checking_relop(sym_entry ** ss, sym_entry * s1, sym_entry * s2);
void print_sym_entry(sym_entry * symp);