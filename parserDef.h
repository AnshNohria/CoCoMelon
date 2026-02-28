/*
 * parserDef.h - Data definitions for data types such as grammar, table, parseTree etc.
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 *
 * Contains all definitions for data types used in parser.c.
 * Keep data definitions in files separate from the files containing
 * function prototypes (as per specification).
 */

#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"

#define MAX_CHILDREN 10

/* ---- Non-terminal symbols ---- */
typedef enum {
    NT_program = 0,
    NT_mainFunction,
    NT_otherFunctions,
    NT_function,
    NT_input_par,
    NT_output_par,
    NT_parameter_list,
    NT_dataType,
    NT_primitiveDatatype,
    NT_constructedDatatype,
    NT_remaining_list,
    NT_stmts,
    NT_typeDefinitions,
    NT_actualOrRedefined,
    NT_typeDefinition,
    NT_fieldDefinitions,
    NT_fieldDefinition,
    NT_fieldType,
    NT_moreFields,
    NT_declarations,
    NT_declaration,
    NT_global_or_not,
    NT_otherStmts,
    NT_stmt,
    NT_assignmentStmt,
    NT_singleOrRecId,
    NT_option_single_constructed,
    NT_oneExpansion,
    NT_moreExpansions,
    NT_funCallStmt,
    NT_outputParameters,
    NT_inputParameters,
    NT_iterativeStmt,
    NT_conditionalStmt,
    NT_elsePart,
    NT_ioStmt,
    NT_arithmeticExpression,
    NT_expPrime,
    NT_term,
    NT_termPrime,
    NT_factor,
    NT_highPrecedenceOperators,
    NT_lowPrecedenceOperators,
    NT_booleanExpression,
    NT_var,
    NT_logicalOp,
    NT_relationalOp,
    NT_returnStmt,
    NT_optionalReturn,
    NT_idList,
    NT_more_ids,
    NT_definetypestmt,
    NT_A,

    NUM_NONTERMINALS
} NonTerminalType;

/* ---- Parse Tree Node ---- */
typedef struct ParseTreeNode {
    int isLeaf;               /* 1 = terminal (leaf), 0 = nonterminal */
    TokenType terminalType;   /* valid if isLeaf == 1 */
    NonTerminalType ntType;   /* valid if isLeaf == 0 */
    char lexeme[MAX_LEXEME_LEN];
    int lineNum;
    struct ParseTreeNode *parent;
    struct ParseTreeNode *children[MAX_CHILDREN];
    int numChildren;
    int ruleNum;              /* which grammar rule expanded this node */
} ParseTreeNode;

#endif /* PARSERDEF_H */