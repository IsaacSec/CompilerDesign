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
  int dval;
  struct _sym_entry *symp;
}

%token <symp> ID
%token SEMI
%token <dval> INTEGER
%token FLOAT
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
%token INT_NUM
%token FLOAT_NUM

/* This solves the dangling else problem */
%nonassoc THEN
%nonassoc ELSE

 /* To avoid ambiguities in the grammar assign associativity */
 /* and preference on the operators */
 
%nonassoc UMINUS

/*  */
%%

program: var_dec stmt_seq   { printf ("No errors in the code\n");}
    ;

var_dec: var_dec single_dec
    |
    ;

single_dec: type ID SEMI
    ;

type: INTEGER
    |   FLOAT
    ;

stmt_seq: stmt_seq stmt 
    |
    ;

stmt: IF exp THEN stmt ELSE stmt
    |   IF exp THEN stmt
    |   WHILE exp DO stmt
    |   variable ASSIGN exp SEMI
    |   READ LPAREN variable RPAREN SEMI
    |   WRITE LPAREN exp RPAREN SEMI
    |   block
    ;

block: LBRACE stmt_seq RBRACE
    ;

exp: simple_exp LT simple_exp
    |   simple_exp EQ simple_exp
    |   simple_exp
    ;

simple_exp: simple_exp PLUS term
    |   simple_exp MINUS term %prec UMINUS
    |   term
    ;

term: term TIMES factor 
    |   term DIV factor
    |   factor
    ;

factor: LPAREN exp RPAREN
    |   INT_NUM
    |   FLOAT_NUM
    |   variable
    ;
    
variable: ID
    ;

%%

/* Bison does NOT implement yyerror, so define it here */
void yyerror (char *msg){
    printf("%d: %s near token '%s'\n", yylineno, msg, yytext);
}

/* GLib Hash functions */

guint hash_func (gconstpointer key) {
    string key_str = (string) key;
    gconstpointer str = key_str;   // Redundant with previus line
    
    return g_str_hash(str);
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
    //g_hash_table_foreach(table, print_hash_table_entry, NULL);
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init (&iter, table);
    
    printf("+------------------------------+\n");

    while (g_hash_table_iter_next (&iter, &key, &value)) {
        sym_entry * entry = (sym_entry*) value;
        printf("%s %s %s %ld\n",(string)key, entry->identifier, entry->type, entry->value);
    }
}

/* Function to manage symbol table */
sym_entry * symlook (string s) {

    if (symtable == NULL) {
        symtable = g_hash_table_new_full(
            hash_func,
            key_equal_func,
            key_destroy_func,
            value_destroy_fun);
    }

    gpointer gp = s;
    
    if (g_hash_table_contains(symtable, gp) == TRUE) {
        //printf("Duplicated\n");
        return g_hash_table_lookup (symtable, gp);
    
    } else {
        sym_entry * entry = (sym_entry *) malloc(sizeof(sym_entry));
        entry->identifier = strdup(s);
        entry->type = _UNDEF;
        entry->value = 0;

        // We duplicate s instead of using entry->identifier as a key to avoid double free
        if (g_hash_table_insert(symtable, strdup(s), entry) == TRUE) {
            //printf("Inserted\n");
            return entry;
        } else {
            return NULL;
        }
    }   
}

/* Bison does NOT define the main entry point so define it here */
int main (){
    
    printf("+----------------------------------+\n");
    printf("|  Lexical and Syntantic analyzer  |\n");
    printf("|      by { Martin and Isaac }     |\n");
    printf("+----------------------------------+\n\n");

    symtable = g_hash_table_new_full(
            hash_func,
            key_equal_func,
            key_destroy_func,
            value_destroy_fun);

    yyparse();

    print_hash_table(symtable);
    return 0;
}