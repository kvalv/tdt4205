%{
#include <stdio.h>
enum { END = 0, WORD, PUNCTUATION, SEPARATOR, NEWLINE };
int chars, words, lines;
%}

%s STRING
word [^ \t\v\n\.]+

%%
<<EOF>>     { return END; }
\n          { return NEWLINE; }
\.          { return PUNCTUATION; }
[ \t\v]+    { return SEPARATOR; }
{word}      { return WORD ; }

%%

int main(void) {
  int token;

  while (token = yylex()) {

    chars += yyleng;

    switch (token) {
        case NEWLINE: {
            lines++;
            break;
        }
        case WORD: {
            words ++;
            break;
        }
    }
  }

  printf("%d %d %d \n",
      lines,
      words,
      chars
  );
  return 0;
}

