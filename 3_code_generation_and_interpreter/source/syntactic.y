%{
    #include "../headers/definitions.h"
    #include "../headers/table.h"
    #include "../headers/typehandle.h"
    #include "../headers/semantic.h"
    #include "../headers/attribute.h"
    
    int yylineno;
    char * yytext;
    
    /* Function definitions */
    void yyerror (char *string);
    
    /* Supress Bison warning */
    int yylex();
%}

%union {
  int ival;
  float fval;
  char * name;
  struct _sym_entry *symp;
  struct _node_attr *attr;
}

%token <name> ID 
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

%type <attr> var_dec
%type <attr> single_dec
%type <name> type
%type <attr> variable
%type <attr> factor
%type <attr> term
%type <attr> exp
%type <attr> simple_exp
%type <attr> stmt_seq
%type <attr> stmt
%type <attr> block

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
                                $$ = create_node_attr(_EMPTY);
                            }
    ;

single_dec: type ID SEMI    {
                                $$ = create_node_attr(_EMPTY);
                                if (check_definition($$, $2, $1)) {
                                    // Continue
                                }
                            }   
    ;

type: INTEGER   { $$ = _INT; }
    |   FLOAT   { $$ = _FLOAT; }
    ;

stmt_seq: stmt_seq stmt         {
                                    $$ = create_node_attr(_EMPTY);
                                    if (check_stmt_seq_type($$, $1, $2)){
                                        // Continue
                                    }
                                }
    |                           {   $$ = create_node_attr(_EMPTY); }
    ;

stmt: IF exp THEN stmt ELSE stmt            {   
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_complex_stmt_type($$, $2, $4, $6)) {
                                                    // Continue
                                                }
                                            }
    |   IF exp THEN stmt                    {
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_stmt_type($$, $2, $4)) {
                                                    // Continue
                                                }
                                            }
    |   WHILE exp DO stmt                   {   
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_stmt_type($$, $2, $4)) {
                                                    // Continue
                                                }
                                            }
    |   variable ASSIGN exp SEMI            {   
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_assign_type($$, $1, $3)) {
                                                    // Continue
                                                }
                                            }
    |   READ LPAREN variable RPAREN SEMI    {
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_read_type($$, $3)){
                                                    // Continue
                                                }
                                            }
    |   WRITE LPAREN exp RPAREN SEMI        {
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_write_type($$, $3)){
                                                    // Continue
                                                }
                                            }
    |   block                               { $$ = $1; }
    ;

block: LBRACE stmt_seq RBRACE       { $$ = $2;}
    ;

exp: simple_exp LT simple_exp       {   
                                        $$ = create_node_attr(_BOOL);
                                        if (check_relop_type($$, $1, $3)) {
                                            // Continue
                                        }
                                    }
    |   simple_exp EQ simple_exp    {   
                                        $$ = create_node_attr(_BOOL);
                                        if (check_relop_type($$, $1, $3)) {
                                            // Continue
                                        }
                                    }
    |   simple_exp                  { $$ = $1; }
    ;

simple_exp: simple_exp PLUS term                {   
                                                    $$ = create_node_attr(_ENTRY);
                                                    if (check_op_type($$, $1, $3)) {
                                                        // Continue
                                                    }
                                                }
    |   simple_exp MINUS term %prec UMINUS      {
                                                    $$ = create_node_attr(_ENTRY); 
                                                    if (check_op_type($$, $1, $3)) {
                                                        // Continue
                                                    }        
                                                }
    |   term                                    { $$ = $1; }    
    ;

term: term TIMES factor     {
                                $$ = create_node_attr(_ENTRY);
                                if (check_op_type($$, $1, $3)) {
                                    // Continue
                                }
                            }
    |   term DIV factor     {   
                                $$ = create_node_attr(_ENTRY);      
                                if (check_op_type($$, $1, $3)) {
                                    // Continue
                                }
                            }    
    |   factor              { $$ = $1; }
    ;

factor: LPAREN exp RPAREN       { $$ = $2; }
    |   INT_NUM                 {
                                    $$ = create_node_attr(_ENTRY);
                                    $$->entry = new_temp(_INT);
                                    $$->entry->value.ival = $1; 
                                }    
    |   FLOAT_NUM               {   
                                    $$ = create_node_attr(_ENTRY);
                                    $$->entry = new_temp(_FLOAT);
                                    $$->entry->value.fval = $1; 
                                }
    |   variable                { $$ = $1; }
    ;
    
variable: ID    {
                    $$ = create_node_attr(_ENTRY);
                    if (check_variable($$, $1)) {
                        // Continue
                    } 
                }
    ;

%%

/* Bison does NOT implement yyerror, so define it here */
void yyerror (char *msg){
    printf("%d: %s near token '%s'\n", yylineno, msg, yytext);
}

/* Semantic funtions implementation */
void semantic_warning(string message){
    printf("[Warning]: (line:%d): %s",yylineno,message);
}

void semantic_error(string message){
    printf("[Error]:   (line:%d): %s",yylineno,message);
}

bool check_variable(node_attr * ss, string identifier) {
    sym_entry * entry = symlook(identifier);

    if (entry == NULL) {
        semantic_error("");
        printf("Variable %s not defined\n",identifier);
        ss->type = _ERROR;
        return false;
    } else {
        ss->entry = entry;
        return true;
    }
}

bool check_definition(node_attr * ss, string identifier, string type) {
    sym_entry * entry = symput(identifier);

    if (entry == NULL) {
        semantic_error("");
        printf("Variable %s is already defined\n",identifier);
        ss->type = _ERROR;
        return false;   
    } else {
        entry->type = type;
        ss->type = _ENTRY;
        return true;
    }
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
    //g_hash_table_destroy(symtable);
    return 0;
}