%{
    #include <string.h>
    #include <stdio.h>
    #include <glib.h>
    #include <stdbool.h>
    #include "table.h"

    /* TYPES */
    const string _FLOAT = "float";
    const string _INT = "integer";
    const string _UNDEF = "undefined";
    const string _BOOL = "boolean";
    const string _ERROR = "error";
    const string _EMPTY = "empty";

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
%type <symp> block

%%

program: var_dec stmt_seq   {
                                if ($1->type == _ERROR || $2->type == _ERROR){
                                    printf ("Semantic errors found\n");
                                } else {
                                    printf ("No errors found\n");
                                } 
                            }
    ;

var_dec: var_dec single_dec { 
                                $$ = $2; 
                            }
    |                       {   
                                $$ = create_temp_entry(_EMPTY);
                            }
    ;

single_dec: type ID SEMI    {
                                $$ = create_temp_entry(_EMPTY);
                                $2->type = $1;
                            }   
    ;

type: INTEGER   { $$ = _INT; }
    |   FLOAT   { $$ = _FLOAT; }
    ;

stmt_seq: stmt_seq stmt         {
                                    if ($1->type == _EMPTY && $2->type == _EMPTY){
                                        $$ = create_temp_entry(_EMPTY);
                                        // Continue
                                    } else {
                                        $$ = create_temp_entry(_ERROR);
                                    }
                                }
    |                           {   $$ = create_temp_entry(_EMPTY); }
    ;

stmt: IF exp THEN stmt ELSE stmt            {   
                                                if ($2->type == _BOOL){
                                                    if ($4->type == _EMPTY && $6->type == _EMPTY){
                                                        $$ = create_temp_entry(_EMPTY);
                                                    } else {
                                                        $$ = create_temp_entry(_ERROR);
                                                    }
                                                } else {
                                                    $$ = create_temp_entry(_ERROR);
                                                }
                                            }
    |   IF exp THEN stmt                    {
                                                if ($2->type == _BOOL){
                                                    $$ = create_temp_entry(_EMPTY);
                                                    // Continue
                                                } else {
                                                    $$ = create_temp_entry(_ERROR); 
                                                }
                                            }
    |   WHILE exp DO stmt                   {   
                                                if ($2->type == _BOOL){
                                                    $$ = create_temp_entry(_EMPTY);
                                                    // Continue
                                                } else {
                                                    $$ = create_temp_entry(_ERROR); 
                                                }
                                            }
    |   variable ASSIGN exp SEMI            {   
                                                //printf("Assigment: "); print_sym_entry($$);
                                                if (type_checking_assign(&$$,$1,$3)) {
                                                    $$ = create_temp_entry(_EMPTY);
                                                    // Continue
                                                } else {
                                                    $$ = create_temp_entry(_ERROR); 
                                                } 
                                            }
    |   READ LPAREN variable RPAREN SEMI    {
                                                if ($3->type == _FLOAT || $3->type == _INT) {
                                                    $$ = create_temp_entry(_EMPTY);
                                                    // Continue;
                                                } else {
                                                    $$ = create_temp_entry(_ERROR); 
                                                    syntantic_error("The variable must be an Integer or Float type\n");
                                                }
                                            }
    |   WRITE LPAREN exp RPAREN SEMI        {
                                                if ($3->type == _FLOAT || $3->type == _INT) {
                                                    $$ = create_temp_entry(_EMPTY);
                                                    // Continue;
                                                } else {
                                                    $$ = create_temp_entry(_ERROR); 
                                                    syntantic_error("The variable must be an Integer or Float type\n");
                                                }
                                            }
    |   block                               { $$ = $1; }
    ;

block: LBRACE stmt_seq RBRACE       { $$ = $2;}
    ;

exp: simple_exp LT simple_exp       {
                                        //printf("Relop: "); print_sym_entry($$); 
                                        if (type_checking_relop(&$$,$1,$3)) {
                                            // Continue
                                        }
                                    }
    |   simple_exp EQ simple_exp    {   // Note ---- Check boolean type
                                        //printf("Reoop: "); print_sym_entry($$);
                                        if (type_checking_relop(&$$,$1,$3)) {
                                            // Continue
                                        }
                                    }
    |   simple_exp                  { $$ = $1; }
    ;

simple_exp: simple_exp PLUS term                {
                                                    if (type_checking_op(&$$,$1,$3)) {
                                                        // Continue
                                                    }
                                                }
    |   simple_exp MINUS term %prec UMINUS      { 
                                                    if (type_checking_op(&$$,$1,$3)) {
                                                        // Continue
                                                    }        
                                                }
    |   term                                    { $$ = $1; }    
    ;

term: term TIMES factor     {   
                                if (type_checking_op(&$$,$1,$3)) {
                                    // Continue
                                }
                            }
    |   term DIV factor     {   
                                if (type_checking_op(&$$,$1,$3)) {
                                    // Continue
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
    printf("Error: (line:%d): %s",yylineno,message);
}

bool type_checking_op(sym_entry ** ss, sym_entry * s1, sym_entry * s2){
    sym_entry * sp = create_temp_entry(_UNDEF);
    *ss = sp;

    if (s1->type == s2->type) {
        sp->type = s1->type;
        return true;
    } else {
        // Type conversion
        if (s1->type == _INT && s2->type == _FLOAT) {
            sp->type = _FLOAT;
            syntantic_warning("conversion INT to FLOAT\n");
            // Value conversion
            return true;
        } else if (s1->type == _FLOAT && s2->type == _INT) {
            sp->type = _FLOAT;
            syntantic_warning("conversion INT to FLOAT\n");
            // Value conversion
            return true;
        } else {
            sp->type = _ERROR;
            syntantic_error("Cannot convert types\n");
            return false;
        }
    }
}

bool type_checking_relop(sym_entry ** ss, sym_entry * s1, sym_entry * s2){
    sym_entry * sp = create_temp_entry(_UNDEF);
    *ss = sp;

    if (s1->type == s2->type) {
        sp->type = _BOOL;
        return true;
    } else {
        // Type conversion 
        if (s1->type == _INT && s2->type == _FLOAT) {
            sp->type = _BOOL;
            syntantic_warning("conversion INT to FLOAT\n");
            // Value conversion
            return true;
        } else if (s1->type == _FLOAT && s2->type == _INT) {
            sp->type = _BOOL;
            syntantic_warning("conversion INT to FLOAT\n");
            // Value conversion
            return true;
        } else {
            sp->type = _ERROR;
            syntantic_error("Cannot convert types\n");
            return false;
        }
    }

    //ss->value=1;
}

bool type_checking_assign(sym_entry ** ss, sym_entry * s1, sym_entry * s2){
    sym_entry * sp = create_temp_entry(_UNDEF);
    *ss = sp;

    if (s1->type == s2->type) {
        sp->type = _EMPTY;
        return true;
    } else {
        // Type conversion 
        if (s1->type == _INT && s2->type == _FLOAT) {
            sp->type = _ERROR;
            // Value conversion
            syntantic_error("Cannot assign Float to INT\n");
            return false;
        } else if (s1->type == _FLOAT && s2->type == _INT) {
            sp->type = _EMPTY;
            // Value conversion
            return true;
        } else {
            sp->type = _ERROR;
            syntantic_error("Cannot convert ");
            printf("%s into %s\n",s1->type,s2->type);
            return false;
        }
    }
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
    entry->identifier = "temp";
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
    printf("%s %s %lu\n",symp->identifier,symp->type,symp->value);
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