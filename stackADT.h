/*
 * stackADT.h - Stack ADT function prototypes
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 */

#ifndef STACKADT_H
#define STACKADT_H

#include "stackADTDef.h"

Stack* createStack(int capacity);
void push(Stack* s, ParseTreeNode* node);
ParseTreeNode* pop(Stack* s);
ParseTreeNode* peek(Stack* s);

#endif /* STACKADT_H */