/*
 * lexer.c - Lexical analyzer implementation
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

/* ---- Internal state ---- */
static char *buffer = NULL;
static int bufLen = 0;
static int pos = 0;
static int lineNum = 1;
static int prevTokenWasDot = 0;

/* ---- Keyword table ---- */
typedef struct {
    const char *keyword;
    TokenType token;
} KeywordEntry;

static KeywordEntry keywordTable[] = {
    {"main",       TK_MAIN},
    {"end",        TK_END},
    {"int",        TK_INT},
    {"real",       TK_REAL},
    {"record",     TK_RECORD},
    {"union",      TK_UNION},
    {"endrecord",  TK_ENDRECORD},
    {"endunion",   TK_ENDUNION},
    {"type",       TK_TYPE},
    {"global",     TK_GLOBAL},
    {"input",      TK_INPUT},
    {"output",     TK_OUTPUT},
    {"parameter",  TK_PARAMETER},
    {"list",       TK_LIST},
    {"definetype", TK_DEFINETYPE},
    {"as",         TK_AS},
    {"while",      TK_WHILE},
    {"endwhile",   TK_ENDWHILE},
    {"if",         TK_IF},
    {"then",       TK_THEN},
    {"else",       TK_ELSE},
    {"endif",      TK_ENDIF},
    {"read",       TK_READ},
    {"write",      TK_WRITE},
    {"call",       TK_CALL},
    {"with",       TK_WITH},
    {"parameters", TK_PARAMETERS},
    {"return",     TK_RETURN},
    {"not",        TK_NOT},
    {"and",        TK_AND},
    {"or",         TK_OR},
    {NULL,         TK_ERROR}
};

/* ---- Terminal name table ---- */
static const char *terminalNames[] = {
    [TK_MAIN]       = "TK_MAIN",
    [TK_END]        = "TK_END",
    [TK_FUNID]      = "TK_FUNID",
    [TK_INT]        = "TK_INT",
    [TK_REAL]       = "TK_REAL",
    [TK_RECORD]     = "TK_RECORD",
    [TK_UNION]      = "TK_UNION",
    [TK_ENDRECORD]  = "TK_ENDRECORD",
    [TK_ENDUNION]   = "TK_ENDUNION",
    [TK_TYPE]       = "TK_TYPE",
    [TK_GLOBAL]     = "TK_GLOBAL",
    [TK_INPUT]      = "TK_INPUT",
    [TK_OUTPUT]     = "TK_OUTPUT",
    [TK_PARAMETER]  = "TK_PARAMETER",
    [TK_LIST]       = "TK_LIST",
    [TK_DEFINETYPE] = "TK_DEFINETYPE",
    [TK_AS]         = "TK_AS",
    [TK_WHILE]      = "TK_WHILE",
    [TK_ENDWHILE]   = "TK_ENDWHILE",
    [TK_IF]         = "TK_IF",
    [TK_THEN]       = "TK_THEN",
    [TK_ELSE]       = "TK_ELSE",
    [TK_ENDIF]      = "TK_ENDIF",
    [TK_READ]       = "TK_READ",
    [TK_WRITE]      = "TK_WRITE",
    [TK_CALL]       = "TK_CALL",
    [TK_WITH]       = "TK_WITH",
    [TK_PARAMETERS] = "TK_PARAMETERS",
    [TK_RETURN]     = "TK_RETURN",
    [TK_NOT]        = "TK_NOT",
    [TK_AND]        = "TK_AND",
    [TK_OR]         = "TK_OR",
    [TK_ID]         = "TK_ID",
    [TK_RUID]       = "TK_RUID",
    [TK_FIELDID]    = "TK_FIELDID",
    [TK_NUM]        = "TK_NUM",
    [TK_RNUM]       = "TK_RNUM",
    [TK_ASSIGNOP]   = "TK_ASSIGNOP",
    [TK_PLUS]       = "TK_PLUS",
    [TK_MINUS]      = "TK_MINUS",
    [TK_MUL]        = "TK_MUL",
    [TK_DIV]        = "TK_DIV",
    [TK_LT]         = "TK_LT",
    [TK_LE]         = "TK_LE",
    [TK_EQ]         = "TK_EQ",
    [TK_GT]         = "TK_GT",
    [TK_GE]         = "TK_GE",
    [TK_NE]         = "TK_NE",
    [TK_SQL]        = "TK_SQL",
    [TK_SQR]        = "TK_SQR",
    [TK_OP]         = "TK_OP",
    [TK_CL]         = "TK_CL",
    [TK_SEM]        = "TK_SEM",
    [TK_COLON]      = "TK_COLON",
    [TK_DOT]        = "TK_DOT",
    [TK_COMMA]      = "TK_COMMA",
    [TK_EOF]        = "TK_EOF",
    [TK_ERROR]      = "TK_ERROR",
    [TK_COMMENT]    = "TK_COMMENT",
    [TK_EPS]        = "TK_EPS"
};

/* ---- Non-terminal name table ---- */
static const char *nonTerminalNames[] = {
    [NT_program]                  = "<program>",
    [NT_mainFunction]             = "<mainFunction>",
    [NT_otherFunctions]           = "<otherFunctions>",
    [NT_function]                 = "<function>",
    [NT_input_par]                = "<input_par>",
    [NT_output_par]               = "<output_par>",
    [NT_parameter_list]           = "<parameter_list>",
    [NT_dataType]                 = "<dataType>",
    [NT_primitiveDatatype]        = "<primitiveDatatype>",
    [NT_constructedDatatype]      = "<constructedDatatype>",
    [NT_remaining_list]           = "<remaining_list>",
    [NT_stmts]                    = "<stmts>",
    [NT_typeDefinitions]          = "<typeDefinitions>",
    [NT_actualOrRedefined]        = "<actualOrRedefined>",
    [NT_typeDefinition]           = "<typeDefinition>",
    [NT_fieldDefinitions]         = "<fieldDefinitions>",
    [NT_fieldDefinition]          = "<fieldDefinition>",
    [NT_fieldType]                = "<fieldType>",
    [NT_moreFields]               = "<moreFields>",
    [NT_declarations]             = "<declarations>",
    [NT_declaration]              = "<declaration>",
    [NT_global_or_not]            = "<global_or_not>",
    [NT_otherStmts]               = "<otherStmts>",
    [NT_stmt]                     = "<stmt>",
    [NT_assignmentStmt]           = "<assignmentStmt>",
    [NT_singleOrRecId]            = "<singleOrRecId>",
    [NT_option_single_constructed]= "<option_single_constructed>",
    [NT_oneExpansion]             = "<oneExpansion>",
    [NT_moreExpansions]           = "<moreExpansions>",
    [NT_funCallStmt]              = "<funCallStmt>",
    [NT_outputParameters]         = "<outputParameters>",
    [NT_inputParameters]          = "<inputParameters>",
    [NT_iterativeStmt]            = "<iterativeStmt>",
    [NT_conditionalStmt]          = "<conditionalStmt>",
    [NT_elsePart]                 = "<elsePart>",
    [NT_ioStmt]                   = "<ioStmt>",
    [NT_arithmeticExpression]     = "<arithmeticExpression>",
    [NT_expPrime]                 = "<expPrime>",
    [NT_term]                     = "<term>",
    [NT_termPrime]                = "<termPrime>",
    [NT_factor]                   = "<factor>",
    [NT_highPrecedenceOperators]  = "<highPrecedenceOperators>",
    [NT_lowPrecedenceOperators]   = "<lowPrecedenceOperators>",
    [NT_booleanExpression]        = "<booleanExpression>",
    [NT_var]                      = "<var>",
    [NT_logicalOp]                = "<logicalOp>",
    [NT_relationalOp]             = "<relationalOp>",
    [NT_returnStmt]               = "<returnStmt>",
    [NT_optionalReturn]           = "<optionalReturn>",
    [NT_idList]                   = "<idList>",
    [NT_more_ids]                 = "<more_ids>",
    [NT_definetypestmt]           = "<definetypestmt>",
    [NT_A]                        = "<A>"
};

const char* getTerminalName(TokenType t) {
    if (t >= 0 && t < NUM_TERMINALS)
        return terminalNames[t];
    return "UNKNOWN_TERMINAL";
}

const char* getNonTerminalName(NonTerminalType nt) {
    if (nt >= 0 && nt < NUM_NONTERMINALS)
        return nonTerminalNames[nt];
    return "UNKNOWN_NONTERMINAL";
}

/* ---- File loading ---- */
static void loadFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (char *)malloc(sz + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(fp);
        exit(1);
    }
    bufLen = (int)fread(buffer, 1, sz, fp);
    buffer[bufLen] = '\0';
    fclose(fp);
}

void initLexer(const char *filename) {
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
    loadFile(filename);
    pos = 0;
    lineNum = 1;
    prevTokenWasDot = 0;
}

void cleanupLexer(void) {
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
    bufLen = 0;
    pos = 0;
    lineNum = 1;
    prevTokenWasDot = 0;
}

void resetLexer(void) {
    pos = 0;
    lineNum = 1;
    prevTokenWasDot = 0;
}

/* ---- Helper: peek character ---- */
static char peekChar(void) {
    if (pos < bufLen) return buffer[pos];
    return '\0';
}

static char advanceChar(void) {
    if (pos < bufLen) {
        char c = buffer[pos++];
        if (c == '\n') lineNum++;
        return c;
    }
    return '\0';
}

/* ---- Skip whitespace ---- */
static void skipWhitespace(void) {
    while (pos < bufLen) {
        char c = buffer[pos];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advanceChar();
        } else {
            break;
        }
    }
}

/* ---- Skip comment (after seeing %) ---- */
static void skipComment(void) {
    /* skip until end of line */
    while (pos < bufLen && buffer[pos] != '\n') {
        pos++;
    }
    /* do not consume newline — let skipWhitespace handle it */
}

/* ---- Lookup keyword ---- */
static TokenType lookupKeyword(const char *word) {
    for (int i = 0; keywordTable[i].keyword != NULL; i++) {
        if (strcmp(word, keywordTable[i].keyword) == 0)
            return keywordTable[i].token;
    }
    return TK_ERROR; /* not a keyword */
}

/* ---- Make a token ---- */
static Token makeToken(TokenType type, const char *lex, int line) {
    Token t;
    t.type = type;
    t.lineNum = line;
    strncpy(t.lexeme, lex, MAX_LEXEME_LEN - 1);
    t.lexeme[MAX_LEXEME_LEN - 1] = '\0';
    return t;
}

/* ---- Main tokenizer ---- */
Token getNextToken(void) {
    skipWhitespace();

    /* Return TK_COMMENT for comment lines */
    if (pos < bufLen && buffer[pos] == '%') {
        int commentLine = lineNum;
        skipComment();
        return makeToken(TK_COMMENT, "%", commentLine);
    }

    if (pos >= bufLen) {
        return makeToken(TK_EOF, "$", lineNum);
    }

    int startLine = lineNum;
    char c = peekChar();

    /* ---- Identifiers starting with # (TK_RUID) ---- */
    if (c == '#') {
        char lex[MAX_LEXEME_LEN];
        int i = 0;
        lex[i++] = advanceChar(); /* # */
        while (pos < bufLen && islower(buffer[pos]) && i < MAX_LEXEME_LEN - 1) {
            lex[i++] = advanceChar();
        }
        lex[i] = '\0';
        prevTokenWasDot = 0;
        return makeToken(TK_RUID, lex, startLine);
    }

    /* ---- Function identifiers starting with _ (TK_FUNID or TK_MAIN) ---- */
    if (c == '_') {
        char lex[MAX_LEXEME_LEN];
        int i = 0;
        lex[i++] = advanceChar(); /* _ */
        while (pos < bufLen && (isalnum(buffer[pos]) || buffer[pos] == '_') && i < MAX_LEXEME_LEN - 1) {
            lex[i++] = advanceChar();
        }
        lex[i] = '\0';
        prevTokenWasDot = 0;
        if (strcmp(lex, "_main") == 0)
            return makeToken(TK_MAIN, lex, startLine);
        if (strlen(lex) > 30) {
            fprintf(stderr, "Lexical Error at line %d: Function Identifier is longer than the prescribed length of 30 characters.\n", startLine);
            lex[30] = '\0';
            return makeToken(TK_ERROR, lex, startLine);
        }
        return makeToken(TK_FUNID, lex, startLine);
    }

    /* ---- Alphabetic: keywords, TK_ID, or TK_FIELDID ---- */
    /* TK_ID pattern:      [b-d][2-7][b-d]*[2-7]*
     * TK_FIELDID pattern: [a-z][a-z]*  (only lowercase letters, no digits)
     * Keywords match FIELDID pattern but are returned as their keyword token */
    if (islower(c)) {
        char lex[MAX_LEXEME_LEN];
        int i = 0;
        int isTkId = 0;

        /* Check if it could be TK_ID: first char in [b-d], second in [2-7] */
        if (c >= 'b' && c <= 'd') {
            lex[i++] = advanceChar(); /* first char [b-d] */
            if (pos < bufLen && buffer[pos] >= '2' && buffer[pos] <= '7') {
                /* TK_ID path */
                isTkId = 1;
                lex[i++] = advanceChar(); /* second char [2-7] */
                /* Read [b-d]* */
                while (pos < bufLen && buffer[pos] >= 'b' && buffer[pos] <= 'd' && i < MAX_LEXEME_LEN - 1) {
                    lex[i++] = advanceChar();
                }
                /* Read [2-7]* */
                while (pos < bufLen && buffer[pos] >= '2' && buffer[pos] <= '7' && i < MAX_LEXEME_LEN - 1) {
                    lex[i++] = advanceChar();
                }
                lex[i] = '\0';
            } else {
                /* First char [b-d] but second not [2-7] → FIELDID path */
                while (pos < bufLen && islower(buffer[pos]) && i < MAX_LEXEME_LEN - 1) {
                    lex[i++] = advanceChar();
                }
                lex[i] = '\0';
            }
        } else {
            /* First char [a-z] but not [b-d] → FIELDID path */
            while (pos < bufLen && islower(buffer[pos]) && i < MAX_LEXEME_LEN - 1) {
                lex[i++] = advanceChar();
            }
            lex[i] = '\0';
        }

        if (isTkId) {
            if (strlen(lex) > 20) {
                fprintf(stderr, "Lexical Error at line %d: Variable Identifier is longer than the prescribed length of 20 characters.\n", startLine);
                lex[20] = '\0';
                prevTokenWasDot = 0;
                return makeToken(TK_ERROR, lex, startLine);
            }
            prevTokenWasDot = 0;
            return makeToken(TK_ID, lex, startLine);
        }

        /* FIELDID path: check keyword first */
        TokenType kwType = lookupKeyword(lex);
        if (kwType != TK_ERROR) {
            prevTokenWasDot = 0;
            return makeToken(kwType, lex, startLine);
        }

        if (strlen(lex) > 20) {
            fprintf(stderr, "Lexical Error at line %d: Variable Identifier is longer than the prescribed length of 20 characters.\n", startLine);
            lex[20] = '\0';
            prevTokenWasDot = 0;
            return makeToken(TK_ERROR, lex, startLine);
        }
        prevTokenWasDot = 0;
        return makeToken(TK_FIELDID, lex, startLine);
    }

    /* ---- Numeric literals (TK_NUM / TK_RNUM) ---- */
    if (isdigit(c)) {
        char lex[MAX_LEXEME_LEN];
        int i = 0;
        while (pos < bufLen && isdigit(buffer[pos]) && i < MAX_LEXEME_LEN - 1) {
            lex[i++] = advanceChar();
        }
        /* Check for real number: digits.digits(E[+-]?digits)? */
        if (pos < bufLen && buffer[pos] == '.' && (pos + 1 < bufLen) && isdigit(buffer[pos + 1])) {
            lex[i++] = advanceChar(); /* . */
            int decDigits = 0;
            while (pos < bufLen && isdigit(buffer[pos]) && i < MAX_LEXEME_LEN - 1) {
                lex[i++] = advanceChar();
                decDigits++;
            }
            if (decDigits != 2) {
                /* Consume any trailing E notation before reporting error */
                if (pos < bufLen && (buffer[pos] == 'E' || buffer[pos] == 'e') && i < MAX_LEXEME_LEN - 1) {
                    int peekOff = 1;
                    if (pos + peekOff < bufLen && (buffer[pos + peekOff] == '+' || buffer[pos + peekOff] == '-'))
                        peekOff++;
                    if (pos + peekOff < bufLen && isdigit(buffer[pos + peekOff]) &&
                        pos + peekOff + 1 < bufLen && isdigit(buffer[pos + peekOff + 1])) {
                        lex[i++] = advanceChar(); /* E */
                        if (pos < bufLen && (buffer[pos] == '+' || buffer[pos] == '-') && i < MAX_LEXEME_LEN - 1)
                            lex[i++] = advanceChar();
                        lex[i++] = advanceChar();
                        lex[i++] = advanceChar();
                    }
                }
                lex[i] = '\0';
                prevTokenWasDot = 0;
                fprintf(stderr, "Lexical Error at line %d: Unknown pattern <%s>\n", startLine, lex);
                return makeToken(TK_ERROR, lex, startLine);
            }
            /* Check for scientific notation E[+-]?[0-9]{2} (exactly 2 exponent digits) */
            if (pos < bufLen && (buffer[pos] == 'E' || buffer[pos] == 'e') && i < MAX_LEXEME_LEN - 1) {
                /* Peek ahead to verify valid E-notation before consuming */
                int peekOff = 1;
                if (pos + peekOff < bufLen && (buffer[pos + peekOff] == '+' || buffer[pos + peekOff] == '-'))
                    peekOff++;
                if (pos + peekOff < bufLen && isdigit(buffer[pos + peekOff]) &&
                    pos + peekOff + 1 < bufLen && isdigit(buffer[pos + peekOff + 1])) {
                    lex[i++] = advanceChar(); /* E */
                    if (pos < bufLen && (buffer[pos] == '+' || buffer[pos] == '-') && i < MAX_LEXEME_LEN - 1) {
                        lex[i++] = advanceChar(); /* + or - */
                    }
                    /* Read exactly 2 exponent digits */
                    lex[i++] = advanceChar();
                    lex[i++] = advanceChar();
                }
            }
            lex[i] = '\0';
            prevTokenWasDot = 0;
            return makeToken(TK_RNUM, lex, startLine);
        }
        lex[i] = '\0';
        prevTokenWasDot = 0;
        return makeToken(TK_NUM, lex, startLine);
    }

    /* ---- Operators and punctuation ---- */
    switch (c) {
        case ':':
            advanceChar();
            if (pos < bufLen && buffer[pos] == '=') {
                advanceChar();
                prevTokenWasDot = 0;
                return makeToken(TK_ASSIGNOP, ":=", startLine);
            }
            prevTokenWasDot = 0;
            return makeToken(TK_COLON, ":", startLine);

        case '<':
            advanceChar();
            if (pos < bufLen && buffer[pos] == '-') {
                if (pos + 1 < bufLen && buffer[pos+1] == '-') {
                    if (pos + 2 < bufLen && buffer[pos+2] == '-') {
                        advanceChar(); advanceChar(); advanceChar();
                        prevTokenWasDot = 0;
                        return makeToken(TK_ASSIGNOP, "<---", startLine);
                    }
                    /* <-- : only 2 dashes */
                    advanceChar(); advanceChar();
                    prevTokenWasDot = 0;
                    fprintf(stderr, "Lexical Error at line %d: Unknown pattern <-->\n", startLine);
                    return makeToken(TK_ERROR, "<--", startLine);
                }
                /* <- : only 1 dash */
                advanceChar();
                prevTokenWasDot = 0;
                fprintf(stderr, "Lexical Error at line %d: Unknown pattern <->\n", startLine);
                return makeToken(TK_ERROR, "<-", startLine);
            }
            if (pos < bufLen && buffer[pos] == '=') {
                advanceChar();
                prevTokenWasDot = 0;
                return makeToken(TK_LE, "<=", startLine);
            }
            prevTokenWasDot = 0;
            return makeToken(TK_LT, "<", startLine);

        case '>':
            advanceChar();
            if (pos < bufLen && buffer[pos] == '=') {
                advanceChar();
                prevTokenWasDot = 0;
                return makeToken(TK_GE, ">=", startLine);
            }
            prevTokenWasDot = 0;
            return makeToken(TK_GT, ">", startLine);

        case '=':
            advanceChar();
            if (pos < bufLen && buffer[pos] == '=') {
                advanceChar();
                prevTokenWasDot = 0;
                return makeToken(TK_EQ, "==", startLine);
            }
            /* single = is an error in this language */
            prevTokenWasDot = 0;
            fprintf(stderr, "Lexical Error at line %d: Unknown symbol '='\n", startLine);
            return makeToken(TK_ERROR, "=", startLine);

        case '!':
            advanceChar();
            if (pos < bufLen && buffer[pos] == '=') {
                advanceChar();
                prevTokenWasDot = 0;
                return makeToken(TK_NE, "!=", startLine);
            }
            prevTokenWasDot = 0;
            fprintf(stderr, "Lexical Error at line %d: Unknown symbol '!'\n", startLine);
            return makeToken(TK_ERROR, "!", startLine);

        case '.':
            advanceChar();
            prevTokenWasDot = 1;
            return makeToken(TK_DOT, ".", startLine);

        case ';':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_SEM, ";", startLine);

        case ',':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_COMMA, ",", startLine);

        case '[':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_SQL, "[", startLine);

        case ']':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_SQR, "]", startLine);

        case '(':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_OP, "(", startLine);

        case ')':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_CL, ")", startLine);

        case '+':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_PLUS, "+", startLine);

        case '-':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_MINUS, "-", startLine);

        case '*':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_MUL, "*", startLine);

        case '/':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_DIV, "/", startLine);

        case '~':
            advanceChar();
            prevTokenWasDot = 0;
            return makeToken(TK_NOT, "~", startLine);

        case '&':
            advanceChar();
            if (pos < bufLen && buffer[pos] == '&') {
                advanceChar();
                if (pos < bufLen && buffer[pos] == '&') {
                    advanceChar();
                    prevTokenWasDot = 0;
                    return makeToken(TK_AND, "&&&", startLine);
                }
                prevTokenWasDot = 0;
                fprintf(stderr, "Lexical Error at line %d: Unknown pattern <&&>\n", startLine);
                return makeToken(TK_ERROR, "&&", startLine);
            }
            prevTokenWasDot = 0;
            fprintf(stderr, "Lexical Error at line %d: Unknown symbol '&'\n", startLine);
            return makeToken(TK_ERROR, "&", startLine);

        case '|': {
            char lex2[2] = {advanceChar(), '\0'};
            prevTokenWasDot = 0;
            fprintf(stderr, "Lexical Error at line %d: Unknown symbol '%s'\n", startLine, lex2);
            return makeToken(TK_ERROR, lex2, startLine);
        }

        case '@':
            advanceChar();
            if (pos + 1 < bufLen && buffer[pos] == '@' && buffer[pos+1] == '@') {
                advanceChar(); advanceChar();
                prevTokenWasDot = 0;
                return makeToken(TK_OR, "@@@", startLine);
            }
            prevTokenWasDot = 0;
            fprintf(stderr, "Lexical Error at line %d: Unknown symbol '@'\n", startLine);
            return makeToken(TK_ERROR, "@", startLine);

        default: {
            char lex[2] = {advanceChar(), '\0'};
            prevTokenWasDot = 0;
            fprintf(stderr, "Lexical Error at line %d: Unknown symbol '%s'\n", startLine, lex);
            return makeToken(TK_ERROR, lex, startLine);
        }
    }
}

/* ---- Option 1: Print comment-free code ---- */
void printCommentFreeCode(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return;
    }
    int ch;
    int inComment = 0;
    while ((ch = fgetc(fp)) != EOF) {
        if (inComment) {
            if (ch == '\n') {
                inComment = 0;
                putchar('\n');
            }
            /* else skip the comment character */
        } else {
            if (ch == '%') {
                inComment = 1;
            } else {
                putchar(ch);
            }
        }
    }
    fclose(fp);
}

/* ---- removeComments: write comment-free code to a file ---- */
void removeComments(const char *testcaseFile, const char *cleanFile) {
    FILE *inFp = fopen(testcaseFile, "r");
    if (!inFp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", testcaseFile);
        return;
    }
    FILE *outFp = fopen(cleanFile, "w");
    if (!outFp) {
        fprintf(stderr, "Error: Cannot open file '%s' for writing\n", cleanFile);
        fclose(inFp);
        return;
    }
    int ch;
    int inComment = 0;
    while ((ch = fgetc(inFp)) != EOF) {
        if (inComment) {
            if (ch == '\n') {
                inComment = 0;
                fputc('\n', outFp);
            }
        } else {
            if (ch == '%') {
                inComment = 1;
            } else {
                fputc(ch, outFp);
            }
        }
    }
    fclose(inFp);
    fclose(outFp);
}

/* ---- Option 2: Print all tokens ---- */
void printAllTokens(const char *filename) {
    initLexer(filename);
    Token t;
    do {
        t = getNextToken();
        if (t.type == TK_EOF) break;
        if (t.type == TK_ERROR) {
            /* error already printed in getNextToken */
            continue;
        }
        printf("Line no. %d\t Lexeme %s\t Token %s\n",
               t.lineNum, t.lexeme, getTerminalName(t.type));
    } while (1);
    cleanupLexer();
}
