## Syntactic Analyzer for tinyC

**IMPORTANT:** If you are using MacOS, change the flag -lfl in CC_FLAGS with -ll  (Makefile)

# Requirements
 - gcc
 - flex/lex
 - bison
 - glib-2.0

### Compile

```
make
```

### Compile & Run

```
make run
```
**note:** this will run test01.c by default

### Run

```
./analyzer.o < test01.c
```
**note:** you can change test01.c with any c source file that you want to test

## Clean

```
make clean
```

## Licence
MIT &copy; IsaacSecundino MartinRuiz
