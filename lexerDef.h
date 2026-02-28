/*
 * lexerDef.h - Data definitions used in lexer.c
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 *
 * Contains all data definitions used in the lexical analyzer module.
 * Keep data definitions in files separate from the files containing
 * function prototypes (as per specification).
 */

#ifndef LEXERDEF_H
#define LEXERDEF_H

#define MAX_LEXEME_LEN 64

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
    TK_ASSIGNOP,  /* <--- or := */
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

/* ---- Token structure (tokenInfo) ---- */
typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_LEN];
    int lineNum;
} Token;

/* Alias for specification compatibility */
typedef Token tokenInfo;

#endif /* LEXERDEF_H */
