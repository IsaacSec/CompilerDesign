%{
    #include "../headers/definitions.h"
    #include "../headers/table.h"
    #include "../headers/typehandle.h"
    #include "../headers/semantic.h"
    
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
                                if (check_definition(&$$, $2, $1)) {
                                    // Continue
                                }
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
                                                if (check_assign_type(&$$,$1,$3)) {
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
                                                    semantic_error("The variable must be an integer or float type\n");
                                                }
                                            }
    |   WRITE LPAREN exp RPAREN SEMI        {
                                                if ($3->type == _FLOAT || $3->type == _INT) {
                                                    $$ = create_temp_entry(_EMPTY);
                                                    // Continue;
                                                } else {
                                                    $$ = create_temp_entry(_ERROR); 
                                                    semantic_error("The variable must be an integer or float type\n");
                                                }
                                            }
    |   block                               { $$ = $1; }
    ;

block: LBRACE stmt_seq RBRACE       { $$ = $2;}
    ;

exp: simple_exp LT simple_exp       {
                                        if (check_relop_type(&$$,$1,$3)) {
                                            // Continue
                                        }
                                    }
    |   simple_exp EQ simple_exp    {   
                                        if (check_relop_type(&$$,$1,$3)) {
                                            // Continue
                                        }
                                    }
    |   simple_exp                  { $$ = $1; }
    ;

simple_exp: simple_exp PLUS term                {
                                                    if (check_op_type(&$$,$1,$3)) {
                                                        // Continue
                                                    }
                                                }
    |   simple_exp MINUS term %prec UMINUS      { 
                                                    if (check_op_type(&$$,$1,$3)) {
                                                        // Continue
                                                    }        
                                                }
    |   term                                    { $$ = $1; }    
    ;

term: term TIMES factor     {   
                                if (check_op_type(&$$,$1,$3)) {
                                    // Continue
                                }
                            }
    |   term DIV factor     {   
                                if (check_op_type(&$$,$1,$3)) {
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
    
variable: ID    { 
                    if (check_variable(&$$, $1)) {
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

bool check_variable(sym_entry ** ss, string identifier) {
    sym_entry * sp = symlook(identifier);

    if (sp == NULL) {
        semantic_error("");
        printf("Variable %s not defined\n",identifier);
        sp = create_temp_entry(_ERROR);    
        *ss = sp;
        return false;
    } else {
        *ss = sp;
        return true;
    }
}

bool check_definition(sym_entry ** ss, string identifier, string type) {
    sym_entry * sp = symput(identifier);

    if (sp == NULL) {
        semantic_error("");
        printf("Variable %s is already defined\n",identifier);
        sp = create_temp_entry(_ERROR); 
        *ss = sp;
        return false;   
    } else {
        sp->type = type;
        sp = create_temp_entry(_EMPTY);
        *ss = sp;
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