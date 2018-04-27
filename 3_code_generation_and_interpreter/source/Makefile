#`pkg-config --cflags --libs glib-2.0`
GLIB=`pkg-config --cflags --libs glib-2.0`
CC=gcc
FLEX=flex
BISON=bison
EXECUTABLE=analyzer.o
SOURCES=syntactic.tab.c lex.yy.c table.c typehandle.c attribute.c quad.c codegen.c interpreter.c
BISON_FLAGS=-d -v
#FLEX_FLAGS=
CC_FLAGS=-DGRAMMAR -lfl -Wall
B_SRC=syntactic.y
B_OUT=syntactic.tab.c
L_SRC=lexical.l
L_OUT=lex.yy.c
INPUT_FILE=../test/test01.c
TRASH_FILES=*.o lex*.c syntactic.tab.* syntactic.output

$(EXECUTABLE): $(SOURCES)
	$(CC) $(SOURCES) -o $(EXECUTABLE) $(GLIB) $(CC_FLAGS)
$(L_OUT): $(L_SRC) $(B_OUT)
	$(FLEX) $(L_SRC)
$(B_OUT): $(B_SRC) 
	$(BISON) $(B_SRC) $(BISON_FLAGS)
run: $(EXECUTABLE) $(INPUT_FILE)
	./$(EXECUTABLE) $(INPUT_FILE)
clean:
	rm $(TRASH_FILES)
