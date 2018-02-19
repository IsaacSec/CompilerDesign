%{
    #include <string.h>
    #include <stdio.h>
    
    int yylineno;
    char * yytext;
    /* Function definitions */
    void yyerror (char *string);
%}

%token ID
%token SEMI
%token INTEGER
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
    |   simple_exp MINUS term
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

/* Bison does NOT define the main entry point so define it here */
int main (){
  yyparse();
  return 0;
}