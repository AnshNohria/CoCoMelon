/*
 * parser.h - Predictive parser and parse tree interface (function prototypes)
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 *
 * Contains function prototype declarations of functions in parser.c.
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "parserDef.h"

/* Parse the token stream and build a parse tree.
 * Returns the root of the parse tree, or NULL on failure.
 * Errors are printed to stderr.
 * The source file should already be loaded via initLexer(). */
ParseTreeNode* parseInputSource(void);

/* Print the parse tree (inorder: leftmost child -> parent -> remaining siblings)
 * to the given file pointer. */
void printParseTree(ParseTreeNode *root, FILE *outFile);

/* Free the parse tree memory */
void freeParseTree(ParseTreeNode *root);

#endif /* PARSER_H */
