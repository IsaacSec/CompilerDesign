analyzer.o: lex.yy.c syntactic.tab.c
	gcc syntactic.tab.c lex.yy.c -o analyzer.o -lfl -Wall -DGRAMMAR
lex.yy.c: lexical.l syntactic.tab.c
	flex lexical.l
syntactic.tab.c: syntactic.y 
	bison -d syntactic.y -v
run: analyzer.o test01.c
	./analyzer.o < test01.c
clean:
	rm *.o lex*.c syntactic.tab.*
