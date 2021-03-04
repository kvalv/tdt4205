%{
#include <stdio.h>
//#include "src/y.tab.h"
#include <vslc.h>
// extern int yylval;

%}

%x SCOMMENT
%x SQUOTE

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
<SQUOTE>\\\"           { yymore(); }
<SQUOTE>\"               { BEGIN(INITIAL); return STRING; }

\/\/                       {  BEGIN(SCOMMENT); yymore(); }
<SCOMMENT>[^\n]      { yymore(); }
<SCOMMENT>\n             { BEGIN(INITIAL); }

[0-9]+                 { return NUMBER; }
[a-zA-Z_][_a-zA-Z0-9]* { return IDENTIFIER; }

[+\-*/=~|^&()<>,]            { return (int) yytext[0]; }

%%

/* int main (void) { */
/*     int token; */
/*     while (token = yylex()) { */
/*         // printf(" %s\n", yytext); */
/*     } */
/* } */
