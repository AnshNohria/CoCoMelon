/*
 * stackADTDef.h - Stack ADT data definitions
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 */

#ifndef STACKADTDEF_H
#define STACKADTDEF_H

#include "parserDef.h"

typedef struct {
    ParseTreeNode **nodes;
    int top;
    int capacity;
} Stack;

#endif /* STACKADTDEF_H */