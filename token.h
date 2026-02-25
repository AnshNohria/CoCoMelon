/*
 * token.h - Token type definitions and data structures for the compiler
 * CS F363 - Compiler Construction
 */

#ifndef TOKEN_H
#define TOKEN_H

#define MAX_LEXEME_LEN 64
#define MAX_CHILDREN 10

/* ---- Terminal tokens ---- */
typedef enum {
    /* Keywords */
    TK_MAIN = 0,
    TK_END,
    TK_FUNID,
    TK_INT,
    TK_REAL,
    TK_RECORD,
    TK_UNION,
    TK_ENDRECORD,
    TK_ENDUNION,
    TK_TYPE,
    TK_GLOBAL,
    TK_INPUT,
    TK_OUTPUT,
    TK_PARAMETER,
    TK_LIST,
    TK_DEFINETYPE,
    TK_AS,
    TK_WHILE,
    TK_ENDWHILE,
    TK_IF,
    TK_THEN,
    TK_ELSE,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_CALL,
    TK_WITH,
    TK_PARAMETERS,
    TK_RETURN,
    TK_NOT,
    TK_AND,
    TK_OR,

    /* Identifiers & literals */
    TK_ID,
    TK_RUID,
    TK_FIELDID,
    TK_NUM,
    TK_RNUM,

    /* Operators */
    TK_ASSIGNOP,  /* := */
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_LT,
    TK_LE,
    TK_EQ,
    TK_GT,
    TK_GE,
    TK_NE,

    /* Punctuation */
    TK_SQL,    /* [ */
    TK_SQR,    /* ] */
    TK_OP,     /* ( */
    TK_CL,     /* ) */
    TK_SEM,    /* ; */
    TK_COLON,  /* : */
    TK_DOT,    /* . */
    TK_COMMA,  /* , */

    /* Special */
    TK_EOF,
    TK_ERROR,
    TK_COMMENT, /* % comment line */
    TK_EPS,     /* epsilon in grammar */

    NUM_TERMINALS
} TokenType;

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

/* ---- Token structure ---- */
typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_LEN];
    int lineNum;
} Token;

/* ---- Parse Tree Node ---- */
typedef struct ParseTreeNode {
    int isLeaf;               /* 1 = terminal, 0 = nonterminal */
    TokenType terminalType;   /* valid if isLeaf == 1 */
    NonTerminalType ntType;   /* valid if isLeaf == 0 */
    char lexeme[MAX_LEXEME_LEN];
    int lineNum;
    struct ParseTreeNode *parent;
    struct ParseTreeNode *children[MAX_CHILDREN];
    int numChildren;
    int ruleNum;              /* which grammar rule expanded this node */
} ParseTreeNode;

/* ---- String name lookup helpers ---- */
const char* getTerminalName(TokenType t);
const char* getNonTerminalName(NonTerminalType nt);

#endif /* TOKEN_H */
