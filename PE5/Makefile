LEX=flex
YACC=bison
YFLAGS+=--defines=src/y.tab.h -o y.tab.c
CFLAGS+=-std=c99 -g -Isrc -Iinclude -D_POSIX_C_SOURCE=200809L -DYYSTYPE="node_t *"
LDLIBS+=-lc

src/vslc: src/vslc.c src/parser.o src/scanner.o src/nodetypes.o src/tree.o src/ir.o src/tlhash.c
src/y.tab.h: src/parser.c
src/scanner.c: src/y.tab.h src/scanner.l
clean:
	-rm -f src/parser.c src/scanner.c src/*.tab.* src/*.o
purge: clean
	-rm -f src/vslc
vsl_programs:
	wget https://folk.ntnu.no/michaeng/tdt4205_21/files/PE5_new_vsl_programs.zip -O examples.zip
	unzip examples.zip 
	mv PE5_new_vsl_programs vsl_programs
	rm examples.zip
