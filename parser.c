/*
 * parser.c - LL(1) Predictive Parser with Parse Tree construction
 * CS F363 - Compiler Construction
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "grammar.h"
#include "token.h"

/* ---- Parse stack ---- */
#define MAX_STACK 4096

typedef struct {
    int isTerminal;
    int symbol;            /* TokenType or NonTerminalType */
    ParseTreeNode *node;   /* corresponding tree node */
} StackEntry;

static StackEntry stack[MAX_STACK];
static int stackTop = -1;

static void stackPush(StackEntry e) {
    if (stackTop >= MAX_STACK - 1) {
        fprintf(stderr, "Parser Error: Stack overflow\n");
        exit(1);
    }
    stack[++stackTop] = e;
}

static StackEntry stackPop(void) {
    if (stackTop < 0) {
        fprintf(stderr, "Parser Error: Stack underflow\n");
        exit(1);
    }
    return stack[stackTop--];
}

static StackEntry stackPeek(void) {
    return stack[stackTop];
}

static int stackEmpty(void) {
    return stackTop < 0;
}

/* ---- Tree node creation ---- */
static ParseTreeNode* createLeafNode(Token tok) {
    ParseTreeNode *node = (ParseTreeNode *)calloc(1, sizeof(ParseTreeNode));
    node->isLeaf = 1;
    node->terminalType = tok.type;
    strncpy(node->lexeme, tok.lexeme, MAX_LEXEME_LEN - 1);
    node->lineNum = tok.lineNum;
    node->numChildren = 0;
    node->parent = NULL;
    node->ruleNum = -1;
    return node;
}

static ParseTreeNode* createNonTerminalNode(NonTerminalType nt) {
    ParseTreeNode *node = (ParseTreeNode *)calloc(1, sizeof(ParseTreeNode));
    node->isLeaf = 0;
    node->ntType = nt;
    node->lexeme[0] = '\0';
    node->lineNum = -1;
    node->numChildren = 0;
    node->parent = NULL;
    node->ruleNum = -1;
    return node;
}

static ParseTreeNode* createEpsilonLeaf(void) {
    ParseTreeNode *node = (ParseTreeNode *)calloc(1, sizeof(ParseTreeNode));
    node->isLeaf = 1;
    node->terminalType = TK_EPS;
    strcpy(node->lexeme, "----");
    node->lineNum = -1;
    node->numChildren = 0;
    node->parent = NULL;
    node->ruleNum = -1;
    return node;
}

/* ---- Error tracking ---- */
static int syntaxErrorCount = 0;

/* ---- Helper: get next meaningful token (skip comments and errors) ---- */
static Token getNextParserToken(void) {
    Token t;
    do {
        t = getNextToken();
    } while (t.type == TK_COMMENT || t.type == TK_ERROR);
    return t;
}

/* ---- Main parsing function ---- */
ParseTreeNode* parseInputSource(void) {
    syntaxErrorCount = 0;

    /* Create root node for <program> */
    ParseTreeNode *root = createNonTerminalNode(NT_program);

    /* Initialize stack: push EOF then start symbol */
    stackTop = -1;
    StackEntry eofEntry;
    eofEntry.isTerminal = 1;
    eofEntry.symbol = TK_EOF;
    eofEntry.node = NULL;
    stackPush(eofEntry);

    StackEntry startEntry;
    startEntry.isTerminal = 0;
    startEntry.symbol = NT_program;
    startEntry.node = root;
    stackPush(startEntry);

    Token currentToken = getNextParserToken();

    while (!stackEmpty()) {
        StackEntry top = stackPeek();

        if (top.isTerminal) {
            /* Terminal on top of stack */
            if (top.symbol == TK_EPS) {
                /* Epsilon: just pop, nothing to match */
                stackPop();
                continue;
            }

            if (top.symbol == (int)currentToken.type) {
                /* Match! */
                stackPop();
                if (top.node) {
                    /* Fill in the leaf node with actual token data */
                    strncpy(top.node->lexeme, currentToken.lexeme, MAX_LEXEME_LEN - 1);
                    top.node->lineNum = currentToken.lineNum;
                }
                if (currentToken.type != TK_EOF) {
                    currentToken = getNextParserToken();
                }
            } else {
                /* Mismatch error */
                syntaxErrorCount++;
                fprintf(stderr, "Syntax Error at line %d: Expected '%s', got '%s' (lexeme: '%s')\n",
                        currentToken.lineNum,
                        getTerminalName((TokenType)top.symbol),
                        getTerminalName(currentToken.type),
                        currentToken.lexeme);
                /* Panic mode: pop the expected terminal and continue */
                stackPop();
            }
        } else {
            /* Nonterminal on top of stack */
            NonTerminalType nt = (NonTerminalType)top.symbol;
            int ruleIdx = parseTable[nt][(int)currentToken.type];

            if (ruleIdx == -1) {
                /* No valid production — syntax error */
                syntaxErrorCount++;
                fprintf(stderr, "Syntax Error at line %d: Unexpected token '%s' (lexeme: '%s') for nonterminal %s\n",
                        currentToken.lineNum,
                        getTerminalName(currentToken.type),
                        currentToken.lexeme,
                        getNonTerminalName(nt));

                /* Panic mode recovery with FOLLOW set:
                 * If current token is in FOLLOW set of the nonterminal,
                 * pop the nonterminal (treat as if it derived epsilon).
                 * Otherwise, skip the current token and retry. */
                if (currentToken.type == TK_EOF || followSet[nt][(int)currentToken.type]) {
                    stackPop();
                } else {
                    currentToken = getNextParserToken();
                }
            } else {
                /* Valid production found */
                stackPop();
                Rule *rule = &grammar[ruleIdx];
                ParseTreeNode *parentNode = top.node;
                parentNode->ruleNum = ruleIdx;

                /* Check if it's an epsilon rule */
                if (rule->rhsLen == 1 && rule->rhs[0].isTerminal && rule->rhs[0].symbol == TK_EPS) {
                    /* Add an epsilon child */
                    ParseTreeNode *epsNode = createEpsilonLeaf();
                    epsNode->parent = parentNode;
                    parentNode->children[parentNode->numChildren++] = epsNode;
                } else {
                    /* Create child nodes and push RHS symbols right-to-left */
                    ParseTreeNode *childNodes[MAX_RHS];
                    for (int i = 0; i < rule->rhsLen; i++) {
                        if (rule->rhs[i].isTerminal) {
                            Token dummy;
                            dummy.type = (TokenType)rule->rhs[i].symbol;
                            strcpy(dummy.lexeme, "");
                            dummy.lineNum = -1;
                            childNodes[i] = createLeafNode(dummy);
                        } else {
                            childNodes[i] = createNonTerminalNode((NonTerminalType)rule->rhs[i].symbol);
                        }
                        childNodes[i]->parent = parentNode;
                        parentNode->children[parentNode->numChildren++] = childNodes[i];
                    }

                    /* Push right-to-left onto stack */
                    for (int i = rule->rhsLen - 1; i >= 0; i--) {
                        StackEntry entry;
                        entry.isTerminal = rule->rhs[i].isTerminal;
                        entry.symbol = rule->rhs[i].symbol;
                        entry.node = childNodes[i];
                        stackPush(entry);
                    }
                }
            }
        }
    }

    if (syntaxErrorCount > 0) {
        fprintf(stderr, "\nParsing completed with %d syntax error(s).\n", syntaxErrorCount);
    } else {
        printf("Input source code is syntactically correct.\n");
    }

    return root;
}

/* ---- Print parse tree (inorder traversal for n-ary tree) ----
 * Inorder: leftmost child -> parent -> remaining children
 *
 * Output format per node:
 * For leaf:    lexeme  lineNum  tokenName  valueIfNumber  parentNT  isLeaf  nodeSymbol
 * For nonterm: ----    ----     ----       ----           parentNT  no      NT_name    ruleNum
 */
static void printNodeInfo(ParseTreeNode *node, FILE *outFile) {
    if (node->isLeaf) {
        /* Leaf node */
        const char *parentName = "ROOT";
        if (node->parent && !node->parent->isLeaf) {
            parentName = getNonTerminalName(node->parent->ntType);
        }

        if (node->terminalType == TK_EPS) {
            fprintf(outFile, "%-20s %-6s %-25s %-15s %-30s yes    ----\n",
                    "----", "----", "TK_EPS", "----", parentName);
        } else if (node->terminalType == TK_NUM) {
            fprintf(outFile, "%-20s %-6d %-25s %-15s %-30s yes    ----\n",
                    node->lexeme, node->lineNum,
                    getTerminalName(node->terminalType),
                    node->lexeme, parentName);
        } else if (node->terminalType == TK_RNUM) {
            fprintf(outFile, "%-20s %-6d %-25s %-15s %-30s yes    ----\n",
                    node->lexeme, node->lineNum,
                    getTerminalName(node->terminalType),
                    node->lexeme, parentName);
        } else {
            fprintf(outFile, "%-20s %-6d %-25s %-15s %-30s yes    ----\n",
                    node->lexeme, node->lineNum,
                    getTerminalName(node->terminalType),
                    "----", parentName);
        }
    } else {
        /* Nonterminal node */
        const char *parentName = "ROOT";
        if (node->parent && !node->parent->isLeaf) {
            parentName = getNonTerminalName(node->parent->ntType);
        }
        fprintf(outFile, "%-20s %-6s %-25s %-15s %-30s no     %s\n",
                "----", "----", "----", "----", parentName,
                getNonTerminalName(node->ntType));
    }
}

/* Inorder traversal: leftmost child -> parent -> remaining children */
static void printParseTreeHelper(ParseTreeNode *node, FILE *outFile) {
    if (node == NULL) return;

    if (node->isLeaf || node->numChildren == 0) {
        /* Leaf: just print it */
        printNodeInfo(node, outFile);
        return;
    }

    /* Print leftmost child first */
    printParseTreeHelper(node->children[0], outFile);

    /* Then print parent (self) */
    printNodeInfo(node, outFile);

    /* Then print remaining children */
    for (int i = 1; i < node->numChildren; i++) {
        printParseTreeHelper(node->children[i], outFile);
    }
}

void printParseTree(ParseTreeNode *root, FILE *outFile) {
    fprintf(outFile, "%-20s %-6s %-25s %-15s %-30s %-6s %s\n",
            "Lexeme", "Line", "Token", "Value", "Parent NT", "Leaf?", "Node Symbol");
    fprintf(outFile, "-----------------------------------------------------------------------------------------------------------\n");
    printParseTreeHelper(root, outFile);
}

/* ---- Free parse tree ---- */
void freeParseTree(ParseTreeNode *root) {
    if (root == NULL) return;
    for (int i = 0; i < root->numChildren; i++) {
        freeParseTree(root->children[i]);
    }
    free(root);
}
