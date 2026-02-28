/*
 * grammar.h - Grammar rules, FIRST/FOLLOW sets, parse table
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 */

#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "lexerDef.h"
#include "parserDef.h"

/* Maximum symbols on RHS of any production */
#define MAX_RHS 10

/* Maximum number of grammar rules */
#define MAX_RULES 120

/* ---- Grammar symbol: either terminal or nonterminal ---- */
typedef struct {
    int isTerminal;  /* 1 = terminal, 0 = nonterminal */
    int symbol;      /* TokenType if terminal, NonTerminalType if nonterminal */
} GrammarSymbol;

/* ---- Production rule ---- */
typedef struct {
    NonTerminalType lhs;
    GrammarSymbol rhs[MAX_RHS];
    int rhsLen;
} Rule;

/* Global grammar data */
extern Rule grammar[];
extern int numRules;
extern int parseTable[NUM_NONTERMINALS][NUM_TERMINALS];
extern int followSet[NUM_NONTERMINALS][NUM_TERMINALS];

/* Initialize grammar: load rules, compute FIRST/FOLLOW, build parse table */
void initGrammar(void);

/* Debug: print FIRST and FOLLOW sets */
void printFirstFollowSets(void);

#endif /* GRAMMAR_H */
