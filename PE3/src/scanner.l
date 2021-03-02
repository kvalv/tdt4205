%{
#include <stdio.h>
#include "src/y.tab.h"
// #include <vslc.h>
extern int yylval;

// enum { EOP = 0, NUMBER, IDENTIFIER, DEF, BLOCK_BEGIN, BLOCK_END, RETURN, PRINT, IF, THEN, ELSE, WHILE, DO, VAR, COMMENT, UNK, PAREN_END, PAREN_START, OP_GT, OP_LT, OP_EQUAL, OP_DIVIDE, OP_MUL, OP_SUBTRACT, OP_ADD, ASSIGNMENT, COMMA, STRING};
%}

%x SCOMMENT
%x SQUOTE
%x ERROR

%option noyywrap
%option array
%option yylineno

%%
def                      { return FUNC; }
print                    { return PRINT; }
return                   { return RETURN; }
continue                { return CONTINUE; }
if                       { return IF; }
then                     { return THEN; }
else                     { return ELSE; }
while                    { return WHILE; }
do                       { return DO; }
begin                    { return OPENBLOCK; }
end                      { return CLOSEBLOCK; }
var                      { return VAR; }
:=                       { return ASSIGNMENT; }
\<\<    { return LSHIFT; }
\>\> {return RSHIFT;}

[\t\n\r\v ]            ;

<INITIAL>\"                           { BEGIN(SQUOTE); yymore(); }
<SQUOTE>[^"]            { yymore(); }
<SQUOTE>\"               { BEGIN(INITIAL); return STRING; }

\/\/                       {  BEGIN(SCOMMENT); yymore(); }
<SCOMMENT>[^\n]      { yymore(); }
<SCOMMENT>\n             { BEGIN(INITIAL); }

[0-9]+                 { return NUMBER; }
[a-zA-Z_][_a-zA-Z0-9]* { return IDENTIFIER; }

[+\-*/=~|^&()<>,]            { return (int) yytext[0]; }

. { BEGIN(ERROR); yymore(); }
<ERROR>[^a-zA-Z0-9_+-/*=] { yymore(); }
<ERROR>. { /* anything else is the beginning of a valid token */
           yyless(yyleng-1);  // push back the last character
           printf("error token: %s\n", yytext);
           BEGIN(INITIAL);
         }

%%

int main (void) {

    int token;
    while (token = yylex()) {
        // printf(" %s\n", yytext);
    }

}
