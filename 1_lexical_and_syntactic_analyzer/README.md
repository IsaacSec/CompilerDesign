## Syntactic Analyzer for tinyC

**IMPORTANT:** If you are using MacOS, change the flag -lfl with -ll in the Makefile

# Requirements
 - gcc
 - flex/lex
 - bison

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
