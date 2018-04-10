%{
    #include <string.h>
    #include <stdio.h>
    #include <glib.h>
    #include "table.h"

    /* TYPES */
    const string _FLOAT = "float";
    const string _INT = "integer";
    const string _UNDEF = "undefined";

    GHashTable * symtable;
    
    int yylineno;
    char * yytext;
    
    /* Function definitions */
    void yyerror (char *string);
%}

%union {
  int ival;
  float fval;
  char * name;
  struct _sym_entry *symp;
}

%token <symp> ID 
%token SEMI
%token <string> INTEGER
%token <string> FLOAT
%token IF
%token THEN
%token ELSE
%token WHILE
%token DO
%token ASSIGN
%token READ
%token LPAREN
%token RPAREN
%token WRITE
%token RBRACE
%token LBRACE
%token LT
%token EQ
%token PLUS
%token MINUS
%token TIMES
%token DIV
%token <ival> INT_NUM
%token <fval> FLOAT_NUM

/* This solves the dangling else problem */
%nonassoc THEN
%nonassoc ELSE

 /* To avoid ambiguities in the grammar assign associativity */
 /* and preference on the operators */
 
%nonassoc UMINUS

/*  */

%type <symp> var_dec
%type <symp> single_dec
%type <name> type
%type <symp> variable
%type <symp> factor
%type <symp> term
%type <symp> exp
%type <symp> simple_exp
%type <symp> stmt_seq
%type <symp> stmt
%type <symp>block

%%

program: var_dec stmt_seq   { printf ("No errors in the code\n");}
    ;

var_dec: var_dec single_dec { 
                                $$ = $2; 
                            }
    |
    ;

single_dec: type ID SEMI    {
                                $$ = $2;
                                $$->type = $1;
                            }   
    ;

type: INTEGER   { $$ = _INT; }
    |   FLOAT   { $$ = _FLOAT; }
    ;

stmt_seq: stmt_seq stmt 
    |
    ;

stmt: IF exp THEN stmt ELSE stmt
    |   IF exp THEN stmt
    |   WHILE exp DO stmt
    |   variable ASSIGN exp SEMI            {
                                                if ($1->type == _UNDEF || $3->type == _UNDEF){
                                                        syntantic_error("one or both terms is undefined\n");
                                                } else {
                                                    if ($1->type == $3->type) {
                                                        $$ = $1;
                                                    } else {
                                                        syntantic_warning("not the same type\n");
                                                    }
                                                }   
                                            }
    |   READ LPAREN variable RPAREN SEMI
    |   WRITE LPAREN exp RPAREN SEMI
    |   block
    ;

block: LBRACE stmt_seq RBRACE
    ;

exp: simple_exp LT simple_exp       { 
                                        if ($1->type == _UNDEF || $3->type == _UNDEF){
                                            syntantic_error("one or both terms is undefined\n");
                                        } else {
                                            if ($1->type == $3->type) {
                                                $$ = create_temp_entry($1->type);
                                            } else {
                                                $$ = create_temp_entry(_FLOAT);
                                            }
                                        }   
                                    }
    |   simple_exp EQ simple_exp    {
                                        if ($1->type == _UNDEF || $3->type == _UNDEF){
                                            syntantic_error("one or both terms is undefined\n");
                                        } else {
                                            if ($1->type == $3->type) {
                                                $$ = create_temp_entry($1->type);
                                            } else {
                                                $$ = create_temp_entry(_FLOAT);
                                            }
                                        }   
                                    }
    |   simple_exp                  { $$ = $1; }
    ;

simple_exp: simple_exp PLUS term                {
                                                    if ($1->type == _UNDEF || $3->type == _UNDEF){
                                                        syntantic_error("one or both terms is undefined\n");
                                                    } else {
                                                        if ($1->type == $3->type) {
                                                            $$ = create_temp_entry($1->type);
                                                        } else {
                                                            $$ = create_temp_entry(_FLOAT);
                                                        }
                                                    }   
                                                }
    |   simple_exp MINUS term %prec UMINUS      { 
                                                    if ($1->type == _UNDEF || $3->type == _UNDEF){
                                                        syntantic_error("one or both terms is undefined\n");
                                                    } else {
                                                        if ($1->type == $3->type) {
                                                            $$ = create_temp_entry($1->type);
                                                        } else {
                                                            $$ = create_temp_entry(_FLOAT);
                                                        }
                                                    }              
                                                }
    |   term                                    { $$ = $1; }    
    ;

term: term TIMES factor     {
                                if ($1->type == _UNDEF || $3->type == _UNDEF){
                                    syntantic_error("one or both terms is undefined\n");
                                } else {
                                    if ($1->type == $3->type) {
                                        
                                        $$ = create_temp_entry($1->type);
                                    } else {
                                        $$ = create_temp_entry(_FLOAT);
                                    }
                                }
                            }
    |   term DIV factor     {   
                                if ($1->type == _UNDEF || $3->type == _UNDEF){
                                    syntantic_error("one or both terms is undefined\n");
                                } else {
                                    if ($1->type == $3->type) {
                                        $$ = create_temp_entry($1->type);
                                    } else {
                                        $$ = create_temp_entry(_FLOAT);
                                    }
                                }
                            }    
    |   factor              { $$ = $1; }
    ;

factor: LPAREN exp RPAREN       { $$ = $2; }
    |   INT_NUM                 { $$ = create_temp_entry(_INT); }    
    |   FLOAT_NUM               { $$ = create_temp_entry(_FLOAT); }
    |   variable                { $$ = $1; }
    ;
    
variable: ID { $$ = $1; }
    ;

%%

/* Bison does NOT implement yyerror, so define it here */
void yyerror (char *msg){
    printf("%d: %s near token '%s'\n", yylineno, msg, yytext);
}

void syntantic_warning(string message){
    printf("Warning (line:%d): %s",yylineno,message);
}

void syntantic_error(string message){
    printf("Error: (line:%d)%s",yylineno,message);
}
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



// Not tested beacause at this point we dont need to remove any item
void key_destroy_func (gpointer data){
    free(data);
}

// Not tested beacause at this point we dont need to remove any item
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
        sym_entry * entry = (sym_entry*) value;
        printf("| %7s | %10s | %14s | %12ld |\n",(string)key, entry->identifier, entry->type, entry->value);
    }

    printf("+---------+------------+----------------+--------------+\n");
}


sym_entry * create_temp_entry(string type){
    sym_entry * entry = (sym_entry *) malloc(sizeof(sym_entry));
    entry->type = type;
    entry->value = 0;
    
    return entry;
}

/* Function to manage symbol table */
sym_entry * symlook (string s) {


    // Tha table hasnt been created
    if (symtable == NULL) {
        symtable = g_hash_table_new_full(
            hash_func,
            key_equal_func,
            key_destroy_func,
            value_destroy_fun);
    }

    gpointer gp = s;
    
    // Search the key in the hash table
    if (g_hash_table_contains(symtable, gp) == TRUE) {

        return g_hash_table_lookup (symtable, gp);
    
    } else {
        // Create a new symbol entry
        sym_entry * entry = (sym_entry *) malloc(sizeof(sym_entry));
        entry->identifier = strdup(s);
        entry->type = _UNDEF;
        entry->value = 0;

        // We duplicate s instead of using entry->identifier as a key to avoid double free
        if (g_hash_table_insert(symtable, strdup(s), entry) == TRUE) {
            return entry;
        } else {
            return NULL;
        }
    }   
}

void print_sym_entry(sym_entry * symp){
    printf("%s %s\n",symp->identifier,symp->type);
}

/* Bison does NOT define the main entry point so define it here */
int main (){
    
    printf("\n+----------------------------------+\n");
    printf("|  Lexical and Syntantic analyzer  |\n");
    printf("|      by { Martin and Isaac }     |\n");
    printf("+----------------------------------+\n\n");

    // Just in case :D
    symtable = g_hash_table_new_full(
            hash_func,
            key_equal_func,
            key_destroy_func,
            value_destroy_fun);

    yyparse();

    print_hash_table(symtable);
    return 0;
}