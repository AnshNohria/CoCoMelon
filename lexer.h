/*
 * lexer.h - Lexical analyzer interface (function prototypes)
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 *
 * Contains function prototype declarations of functions in lexer.c.
 * Note: Do not include lexer.h in lexer.c, as lexer.c already has
 * its own function details.
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "lexerDef.h"

/* Initialize the lexer with the given source file */
void initLexer(const char *filename);

/* Free lexer resources */
void cleanupLexer(void);

/* Reset the lexer to the beginning (for re-scanning) */
void resetLexer(void);

/* Get the next token from the input stream */
Token getNextToken(void);

/* Option 1: Print comment-free source code to console */
void printCommentFreeCode(const char *filename);

/* Remove comments and write clean code to a file */
void removeComments(const char *testcaseFile, const char *cleanFile);

/* Option 2: Print all tokens with pretty formatting */
void printAllTokens(const char *filename);

/* String name lookup helpers */
const char* getTerminalName(TokenType t);
const char* getNonTerminalName(NonTerminalType nt);

#endif /* LEXER_H */
