CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
SRCS = driver.c lexer.c grammar.c parser.c stackADT.c
OBJS = $(SRCS:.c=.o)
TARGET = stage1exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

driver.o: driver.c token.h lexerDef.h parserDef.h lexer.h grammar.h parser.h
	$(CC) $(CFLAGS) -c driver.c -o driver.o

lexer.o: lexer.c token.h lexerDef.h parserDef.h
	$(CC) $(CFLAGS) -c lexer.c -o lexer.o

grammar.o: grammar.c grammar.h token.h lexerDef.h parserDef.h
	$(CC) $(CFLAGS) -c grammar.c -o grammar.o

parser.o: parser.c parser.h grammar.h lexer.h token.h lexerDef.h parserDef.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

stackADT.o: stackADT.c stackADT.h stackADTDef.h parserDef.h lexerDef.h
	$(CC) $(CFLAGS) -c stackADT.c -o stackADT.o

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
