%{
#include <stdio.h>
enum { END = 0, WORD, PUNCTUATION, SEPARATOR, NEWLINE };
int nb_matched_strings; /* Example: 'the "dog" sat in a "tree" ' -> nb_matched_strings == 2 */
int tot_chars_inside_quotes = 0;
int inside_quotes_b = 0; /* flag to specify whether we're 'inside' of a string or not */
int chars, words, lines;
%}

%s STRING
word [^ \t\v\n\."]+

%%
<INITIAL>\" {
    BEGIN(STRING);
    inside_quotes_b = 1;
    chars ++;
}
<STRING>\" {
    BEGIN(INITIAL);
    nb_matched_strings ++;
    inside_quotes_b = 0;
    chars ++;
}

<<EOF>>     { return END; }
\n          { return NEWLINE; }
\.          { return PUNCTUATION; }
[ \t\v]+    { return SEPARATOR; }
{word}      { return WORD ; }

%%

int main(void) {
  int token;

  while (token = yylex()) {

    if (inside_quotes_b) {
        tot_chars_inside_quotes += yyleng;
    }

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

  float avg_strlen = nb_matched_strings ? tot_chars_inside_quotes / (float) nb_matched_strings : 0.00f;
  printf("%d %d %d %d %.2f\n",
      lines,
      words,
      chars,
      nb_matched_strings,
      avg_strlen
  );
  return 0;
}

