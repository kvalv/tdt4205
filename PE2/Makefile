# Makefile for first lex example
CC = gcc 
LEX = flex
yacc = bison
CFLAGS = -g

TESTFILES_URL="http://folk.ntnu.no/michaeng/tdt4205_21/files/Compilers_PE2_testcases.tar.gz"

all: prg-2.1 prg-2.2

output.txt: compilers-pe2 testfiles
	./run-tests | tee output.txt


prg-2.1: task_a.l
	lex $<
	gcc -o $@ lex.yy.c -I. -ll

prg-2.2: compilers-pe2.l
	lex $<
	gcc -o $@ lex.yy.c -I. -ll

compilers-pe2: prg-2.2
	ln --symbolic $< $@

clean:
	-@rm -f lex.yy.c compilers-pe2  prg-2.1 prg-2.2
	-@rm -rf Compilers_PE2_testcases

testfiles:
	wget -c ${TESTFILES_URL} -O - | tar -xz

.PHONY: clean testfiles
