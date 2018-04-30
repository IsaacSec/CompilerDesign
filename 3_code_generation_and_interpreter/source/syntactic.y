%{
    #include "../headers/definitions.h"
    #include "../headers/table.h"
    #include "../headers/typehandle.h"
    #include "../headers/semantic.h"
    #include "../headers/attribute.h"
    #include "../headers/codegen.h"
    #include "../headers/quad.h"
    #include "../headers/interpreter.h"
    
    int yylineno;
    char * yytext;
    bool inheretedError = false;
    
    /* Function definitions */
    void yyerror (char *string);
    
    FILE *yyin;

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
%type <attr> n
%type <attr> m

%%

program: var_dec stmt_seq m {
                                backpatch($2->next_list, $3->quad); 
                                printf("\n");

                                printf("\n----- Quad List -----\n");
                                print_quads($2->quad_list);

                                if ($1->type == _ERROR || $2->type == _ERROR || inheretedError){
                                    printf ("\nSemantic errors found\n");
                                } else {
                                    printf ("\nNo errors found\n");
                                    printf("\n\n---- Interpreter ----\n");
                                    interpreter($2->quad_list);
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

stmt_seq: stmt_seq m stmt         {
                                    $$ = create_node_attr(_EMPTY);

                                    if (check_stmt_seq_type($$, $1, $3)){
                                        backpatch($1->next_list, $2->quad);  // Generates warnings: not a JUMP quad
                                        $$->quad_list = merge($$->quad_list, $1->quad_list);
                                        $$->quad_list = merge($$->quad_list, $3->quad_list);
                                        $$->next_list = $3->next_list;
                                    } else {
                                        inheretedError = true;
                                        $$->quad_list = $1->quad_list;
                                        $$->next_list = $1->next_list;
                                    }
                                    
                                }
    |                           {   $$ = create_node_attr(_EMPTY); }
    ;

stmt: IF exp THEN m stmt n stmt     {
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_complex_stmt_type($$, $2, $5, $7)) {
                                                    gen_if_then_else_quad_list($$, $2, $4->quad, $5, $6, $6->quad, $7);
                                                } else {
                                                    remove_error_attr_lines(3, $2, $5, $7);
                                                }
                                            }
    |   IF exp THEN m stmt                    {
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_stmt_type($$, $2, $5)) {
                                                    gen_if_then_quad_list($$, $2, $4->quad, $5);
                                                } else {
                                                    remove_error_attr_lines(2, $2, $5);
                                                }
                                            }
    |   WHILE m exp DO m stmt                   {   
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_stmt_type($$, $3, $6)) {
                                                    gen_while_quad_list($$, $2->quad, $3, $5->quad, $6);
                                                } else {
                                                    remove_error_attr_lines(2, $3, $6);
                                                }
                                            }
    |   variable ASSIGN exp SEMI            {   
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_assign_type($$, $1, $3)) {
                                                    gen_op_quad_list($$, $1, $3, ASSIGN_TO);
                                                } else {
                                                    remove_error_attr_lines(2, $1, $3);
                                                }
                                            }
    |   READ LPAREN variable RPAREN SEMI    {
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_read_type($$, $3)){
                                                    gen_read_quad_list($$, $3);
                                                }
                                            }
    |   WRITE LPAREN exp RPAREN SEMI        {
                                                $$ = create_node_attr(_EMPTY);
                                                if (check_write_type($$, $3)){
                                                    gen_write_quad_list($$, $3);
                                                }
                                            }
    |   block                               { $$ = $1; }
    ;

block: LBRACE stmt_seq RBRACE       { $$ = $2;}
    ;

m:                  {
                        $$ = create_node_attr(_EMPTY);
                        $$->quad = get_next_quad();
                    }
    ;


n:  ELSE m          {
                        $$ = create_node_attr(_EMPTY);
                        quad * q = create_procedure_quad(next_quad(), Q_JUMP, JUMP, NULL, NULL, 0);
                        $$->quad = get_next_quad();
                        $$->quad_list = new_list(q);
                        $$->next_list = new_list(q);
                    }
    ;

exp: simple_exp LT simple_exp       {   
                                        $$ = create_node_attr(_BOOL);
                                        if (check_relop_type($$, $1, $3)) {
                                            gen_relop_quad_list($$, $1, $3, JLT);
                                        } else {
                                            remove_error_attr_lines(2, $1, $3);
                                        }
                                    }
    |   simple_exp EQ simple_exp    {   
                                        $$ = create_node_attr(_BOOL);
                                        if (check_relop_type($$, $1, $3)) {
                                            gen_relop_quad_list($$, $1, $3, JE);
                                        } else {
                                            remove_error_attr_lines(2, $1, $3);
                                        }
                                    }
    |   simple_exp                  { $$ = $1; }
    ;

simple_exp: simple_exp PLUS term                {   
                                                    $$ = create_node_attr(_ENTRY);
                                                    if (check_op_type($$, $1, $3)) {
                                                        gen_op_quad_list($$, $1, $3, ADDITION);
                                                    } else {
                                                        remove_error_attr_lines(2, $1, $3);
                                                    }
                                                }
    |   simple_exp MINUS term %prec UMINUS      {
                                                    $$ = create_node_attr(_ENTRY); 
                                                    if (check_op_type($$, $1, $3)) {
                                                        gen_op_quad_list($$, $1, $3, SUBTRACTION);
                                                        //print_quads($$->quad_list);
                                                    } else {
                                                        remove_error_attr_lines(2, $1, $3);
                                                    }      
                                                }
    |   term                                    { $$ = $1; }    
    ;

term: term TIMES factor     {
                                $$ = create_node_attr(_ENTRY);
                                if (check_op_type($$, $1, $3)) {
                                    gen_op_quad_list($$, $1, $3, MULTIPLICATION);
                                    //print_quads($$->quad_list);
                                } else {
                                    remove_error_attr_lines(2, $1, $3);
                                }
                            }
    |   term DIV factor     {   
                                $$ = create_node_attr(_ENTRY);      
                                if (check_op_type($$, $1, $3)) {
                                    gen_op_quad_list($$, $1, $3, DIVISION);
                                } else {
                                    remove_error_attr_lines(2, $1, $3);
                                }
                            }    
    |   factor              { $$ = $1; }
    ;

factor: LPAREN exp RPAREN       { $$ = $2; }
    |   INT_NUM                 {
                                    $$ = create_node_attr(_ENTRY);
                                    v_value value;
                                    value.ival = $1;
                                    quad * q = gen_temp_constant_quad(_INT, value);
                                    $$->entry = q->f1.dest;
                                    $$->quad_list = new_list(q);
                                }    
    |   FLOAT_NUM               {   
                                    $$ = create_node_attr(_ENTRY);
                                    v_value value;
                                    value.fval = $1;
                                    quad * q = gen_temp_constant_quad(_FLOAT, value);
                                    $$->entry = q->f1.dest;
                                    $$->quad_list = new_list(q);
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
int main (int argc, char **argv){
    
    

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
    
    yyin = fopen(argv[1], "r");
    yyparse();
    fclose(yyin);

    print_hash_table(symtable);
    //g_hash_table_destroy(symtable);
    return 0;
}