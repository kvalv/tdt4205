# Makefile for first lex example
CC = gcc 
LEX = flex
yacc = bison
CFLAGS = -g

TESTFILES_URL="http://folk.ntnu.no/michaeng/tdt4205_21/files/Compilers_PE2_testcases.tar.gz"

output.txt: compilers-pe2 testfiles
	./run-tests | tee output.txt

all: compilers-pe2

lex.yy.c: compilers-pe2.l
	lex compilers-pe2.l

compilers-pe2: lex.yy.c 
	gcc -o $@ $< -I. -ll

clean:
	-@rm lex.yy.c compilers-pe2  

testfiles:
	wget -c ${TESTFILES_URL} -O - | tar -xz
