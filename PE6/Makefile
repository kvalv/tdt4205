LEX=flex
YACC=bison
YFLAGS+=--defines=src/y.tab.h -o y.tab.c
CFLAGS+=-std=c99 -g -Isrc -Iinclude -D_POSIX_C_SOURCE=200809L -DYYSTYPE="node_t *"
LDLIBS+=-lc

src/vslc: src/vslc.c src/parser.o src/scanner.o src/nodetypes.o src/tree.o src/ir.o src/tlhash.c src/generator.o
src/y.tab.h: src/parser.c
src/scanner.c: src/y.tab.h src/scanner.l
clean:
	-rm -f src/parser.c src/scanner.c src/*.tab.* src/*.o
purge: clean
	-rm -f src/vslc

prog: src/vslc vsl_programs/easy.vsl
	src/vslc < vsl_programs/easy.vsl | tee 0prog.s 
	cat 0prog.s | grep -v DEBUG | awk '/.data/,EOF' > prog.s
	rm 0prog.s
	as -c -g -o prog.o prog.s
	cc -o prog prog.o

print: src/vslc
	src/vslc < vsl_programs/easy.vsl
