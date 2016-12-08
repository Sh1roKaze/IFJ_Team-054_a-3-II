CC=gcc -std=c99 

all: ifj16_team54 clean

ifj16_team54 : main.o sematics.o interpret.o stack.o mem_management.o ifj16.o ial.o lex_anal.o parser.o
	$(CC) main.o sematics.o interpret.o stack.o mem_management.o ifj16.o ial.o lex_anal.o parser.o -o  ifj16_team54
     
main.o : main.c
	$(CC) -c main.c

sematics.o : sematics.c sematics.h
	$(CC) -c sematics.c

interpret.o : interpret.c interpret.h
	$(CC) -c interpret.c

stack.o : stack.c stack.h
	$(CC) -c stack.c

mem_management.o : mem_management.c mem_management.h
	$(CC) -c mem_management.c

ifj16.o : ifj16.c ifj16.h
	$(CC) -c ifj16.c

ial.o : ial.c ial.h

lex_anal.o : lex_anal.c lex_anal.h
	$(CC) -c lex_anal.c

parser.o : parser.c parser.h
	$(CC) -c parser.c

clean :
	rm main.o sematics.o interpret.o stack.o mem_management.o ifj16.o ial.o lex_anal.o parser.o

test : all
	sh tests/test.sh | grep -v "ifj16 - OK"

