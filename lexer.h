/*
 * lexer.h - Lexical analyzer interface
 * CS F363 - Compiler Construction
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"

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

/* Option 2: Print all tokens with pretty formatting */
void printAllTokens(const char *filename);

#endif /* LEXER_H */
