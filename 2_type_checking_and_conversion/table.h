
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

/* Function prototype for the symbol table look up routine */
sym_entry *symlook(string s);
sym_entry * create_temp_entry(string type);