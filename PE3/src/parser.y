%{
#include <vslc.h>

void* malloc_text(char* s) {
    char *out = malloc(strlen(s) + 1);
    strcpy(out, s);
    return out;
}
void* dup_yytext() {
    return malloc_text(yytext);
}

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

// TODO: support `if then statement`

%%
program : global_list {
        root = (node_t *) malloc ( sizeof(node_t) );
        node_init ( root, PROGRAM, NULL, 1, $1);
      }

global_list 
    : global { $$ = new(); node_init($$, GLOBAL_LIST, NULL, 1, $1);}
    | global_list global { $$ = new(); node_init($$, GLOBAL_LIST, NULL, 2, $1, $2);}

global 
    : function { $$ = new(); node_init($$, GLOBAL, NULL, 1, $1);}
    | declaration { $$ = new(); node_init($$, GLOBAL, NULL, 1, $1);}

statement_list 
    : statement { $$ = new(); node_init($$, STATEMENT_LIST, NULL, 1, $1);}
    | statement_list statement { $$ = new(); node_init($$, STATEMENT_LIST, NULL, 2, $1, $2);}

print_list 
    : print_item { $$ = new(); node_init($$, PRINT_LIST, NULL, 1, $1);}
    | print_list ',' print_item { $$ = new(); node_init($$, PRINT_LIST, NULL, 2, $1, $3);}

expression_list 
    : expression { $$ = new(); node_init($$, EXPRESSION_LIST, NULL, 1, $1);}
    | expression_list ',' expression { $$ = new(); node_init($$, EXPRESSION_LIST, NULL, 2, $1, $3);}

variable_list 
    : identifier  { $$ = new(); node_init( $$, VARIABLE_LIST, NULL, 1, $1 ); }
    | variable_list ',' identifier { $$ = new(); node_init( $$, VARIABLE_LIST, NULL, 2, $1, $3 ); }

argument_list 
    : expression_list { $$ = new(); node_init($$, ARGUMENT_LIST, NULL, 1, $1);}
    | %empty { $$ = new(); node_init($$, ARGUMENT_LIST, NULL, 0);}

parameter_list 
    : variable_list { $$ = new(); node_init($$, PARAMETER_LIST, NULL, 1, $1);}
    | %empty { $$ = new(); node_init($$, PARAMETER_LIST, NULL, 0);}

declaration_list 
    : declaration { $$ = new(); node_init($$, DECLARATION_LIST, NULL, 1, $1);}
    | declaration_list declaration { $$ = new(); node_init($$, DECLARATION_LIST, NULL, 2, $1, $2);}

function 
    : FUNC identifier '(' parameter_list ')' statement { $$ = new(); node_init($$, FUNCTION, NULL, 3, $2, $4, $6);}

statement_without_if 
    : assignment_statement { $$ = new(); node_init($$, STATEMENT, NULL, 1, $1);}
    | return_statement  { $$ = new(); node_init($$, STATEMENT, NULL, 1, $1);}
    | print_statement  { $$ = new(); node_init($$, STATEMENT, NULL, 1, $1);}
    | if_else_statement { $$ = new(); node_init($$, STATEMENT, NULL, 1, $1);}
    | while_statement  { $$ = new(); node_init($$, STATEMENT, NULL, 1, $1);}
    | null_statement  { $$ = new(); node_init($$, STATEMENT, NULL, 1, $1);}
    | block { $$ = new(); node_init($$, STATEMENT, NULL, 1, $1);}

statement
    : statement_without_if { $$ = $1;}
    | if_statement  { $$ = $1;}

block 
    : OPENBLOCK declaration_list statement_list CLOSEBLOCK { $$ = new(); node_init($$, BLOCK, NULL, 2, $2, $3);}
    | OPENBLOCK statement_list CLOSEBLOCK { $$ = new(); node_init($$, BLOCK, NULL, 1, $2);}

assignment_statement 
    : identifier ASSIGNMENT expression { $$ = new(); node_init($$, ASSIGNMENT_STATEMENT, NULL, 2, $1, $3);}


return_statement 
    : RETURN expression { $$ = new(); node_init($$, RETURN_STATEMENT, NULL, 1, $2);}


print_statement 
    : PRINT print_list { $$ = new(); node_init($$, PRINT_STATEMENT, NULL, 1, $2);}


null_statement 
    : CONTINUE { $$ = new(); node_init($$, NULL_STATEMENT, NULL, 0);}


if_statement 
    : IF relation THEN statement { $$ = new(); node_init($$, IF_STATEMENT, NULL, 2, $2, $4);}

if_else_statement
    : IF relation THEN statement_without_if ELSE statement { $$ = new(); node_init($$, IF_STATEMENT, NULL, 3, $2, $4, $6);}

while_statement 
    : WHILE relation DO statement { $$ = new(); node_init($$, WHILE_STATEMENT, NULL, 2, $2, $4);}

relation 
    : expression '=' expression { $$ = new(); node_init($$, RELATION, NULL, 2, $1, $3);}
    | expression '<' expression  { $$ = new(); node_init($$, RELATION, NULL, 2, $1, $3);}
    | expression '>' expression { $$ = new(); node_init($$, RELATION, NULL, 2, $1, $3);}

expression 
    : expression '|' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("|") , 2, $1, $3);}
    | expression '^' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("^"), 2, $1, $3);}
    | expression '&' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("&"), 2, $1, $3);}
    | expression LSHIFT expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("<<"), 2, $1, $3);}
    | expression RSHIFT expression { $$ = new(); node_init($$, EXPRESSION, malloc_text(">>"), 2, $1, $3);}
    | expression '+' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("+"), 2, $1, $3);}
    | expression '-' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("-"), 2, $1, $3);}
    | expression '*' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("*"), 2, $1, $3);}
    | expression '/' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("/"), 2, $1, $3);}
    | '-' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("-"), 1, $2);}
    | '~' expression { $$ = new(); node_init($$, EXPRESSION, malloc_text("~"), 1, $2);}
    | '(' expression ')' { $$ = new(); node_init($$, EXPRESSION, malloc_text("()"), 1, $2);}
    | number { $$ = new(); node_init($$, EXPRESSION, malloc_text("number"), 1, $1);}
    | identifier { $$ = new(); node_init($$, EXPRESSION, malloc_text("id"), 1, $1);}
    | identifier '(' argument_list ')'{ $$ = new(); node_init($$, EXPRESSION, malloc_text("(...)"), 2, $1, $3);}

declaration 
    : VAR variable_list { $$ = new(); node_init( $$, DECLARATION, NULL, 1, $2 ); }

print_item 
    : expression  
    | string { $$ = new(); node_init($$, PRINT_ITEM, NULL, 1, $1);}

identifier 
    : IDENTIFIER { $$ = new(); node_init( $$, IDENTIFIER_DATA, dup_yytext(), 0); }

number 
    : NUMBER { $$ = new(); node_init( $$, NUMBER_DATA, dup_yytext(), 0); }

string 
    : STRING { $$ = new(); node_init( $$, STRING_DATA, dup_yytext(), 0); }


%%

int
yyerror ( const char *error )
{
    fprintf ( stderr, "%s on line %d\n", error, yylineno );
    exit ( EXIT_FAILURE );
}
