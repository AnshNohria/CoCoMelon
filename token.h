/*
 * token.h - Unified token/parser type definitions and helper declarations
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 *
 * This header provides convenience access to all type definitions
 * by including both lexerDef.h and parserDef.h, plus declaring
 * the name-lookup helper functions implemented in lexer.c.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "lexerDef.h"
#include "parserDef.h"

/* ---- String name lookup helpers (implemented in lexer.c) ---- */
const char* getTerminalName(TokenType t);
const char* getNonTerminalName(NonTerminalType nt);

#endif /* TOKEN_H */
