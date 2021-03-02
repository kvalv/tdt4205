%{
#include <vslc.h>
%}
%left '|'
%left '^'
%left '&'
%left LSHIFT RSHIFT
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS
%right '~'
	//%expect 1

%token FUNC PRINT RETURN CONTINUE IF THEN ELSE WHILE DO OPENBLOCK CLOSEBLOCK
%token VAR NUMBER IDENTIFIER STRING LSHIFT RSHIFT ASSIGNMENT

%%
program : global_list {
        root = (node_t *) malloc ( sizeof(node_t) );
        node_init ( root, PROGRAM, NULL, 0 );
      }

global_list 
    : global
    |  global_list global

global 
    : function 
    |  declaration

statement_list 
    : statement 
    | statement_list statement

print_list 
    : print_item 
    |  print_list ’,’ print_item

expression_list 
    : expression 
    |  expression_list ’,’ expression

variable_list 
    : identifier 
    |  variable_list ’,’ identifier

argument_list 
    : expression_list 
    |  %empty

parameter_list 
    : variable_list 
    |  %empty

function 
    : DEF identifier ’(’ parameter_list ’)’ statement

statement 
    : assignment_statement 
    | return_statement 
    | print_statement 
    | if_statement 
    | while_statement 
    | null_statement 
    | block

block 
    : BEGIN declaration_list statement_list END 
    | BEGIN statement_list END

assign_statement 
    : identifier ’:=’ expression


return_statement 
    : RETURN expression


print_statement 
    : PRINT print_list


null_statement 
    : CONTINUE


if_statement 
    : IF relation THEN statement


if_statement 
    : IF relation THEN statement else statement


whilestatement 
    : WHILE relation DO statement

relation 
    : expression ’=’ expression 
    | expression ’<’ expression 
    | expression ’>’ expression

expression 
    : expression ’|’ expression
    | expression ’^’ expression
    | expression ’&’ expression
    | expression ’<<’ expression
    | expression ’>>’ expression
    | expression ’+’ expression
    | expression ’-’ expression
    | expression ’*’ expression
    | expression ’/’ expression
    | ’-’ expression
    | ’~’ expression
    | ’(’ expression ’)’
    | number 
    | identifier 
    | identifier ’(’ argument_list ’)’

declaration 
    : var variable_list

printitem 
    : expression 
    | string

identifier 
    : IDENTIFIER

number 
    : NUMBER

string 
    : STRING



%%

int
yyerror ( const char *error )
{
    fprintf ( stderr, "%s on line %d\n", error, yylineno );
    exit ( EXIT_FAILURE );
}
