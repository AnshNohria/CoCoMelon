/*
 * grammar.c - Grammar rules, automated FIRST/FOLLOW computation, parse table
 * CS F363 - Compiler Construction
 */

#include <stdio.h>
#include <string.h>
#include "grammar.h"
#include "token.h"

/* ---- Rule storage ---- */
Rule grammar[MAX_RULES];
int numRules = 0;
int parseTable[NUM_NONTERMINALS][NUM_TERMINALS];

/* ---- FIRST and FOLLOW sets ---- */
/* Each set is a boolean array indexed by TokenType */
static int firstSet[NUM_NONTERMINALS][NUM_TERMINALS];
int followSet[NUM_NONTERMINALS][NUM_TERMINALS];
/* Whether a nonterminal can derive epsilon */
static int nullable[NUM_NONTERMINALS];

/* ---- Helper: create a terminal grammar symbol ---- */
static GrammarSymbol T(TokenType t) {
    GrammarSymbol gs;
    gs.isTerminal = 1;
    gs.symbol = t;
    return gs;
}

/* ---- Helper: create a nonterminal grammar symbol ---- */
static GrammarSymbol NT(NonTerminalType nt) {
    GrammarSymbol gs;
    gs.isTerminal = 0;
    gs.symbol = nt;
    return gs;
}

/* ---- Add a rule ---- */
static int addRuleArr(NonTerminalType lhs, GrammarSymbol *rhs, int len) {
    int idx = numRules++;
    grammar[idx].lhs = lhs;
    grammar[idx].rhsLen = len;
    for (int i = 0; i < len; i++)
        grammar[idx].rhs[i] = rhs[i];
    return idx;
}

/* ---- Helper: add epsilon rule ---- */
static int addEpsRule(NonTerminalType lhs) {
    int idx = numRules++;
    grammar[idx].lhs = lhs;
    grammar[idx].rhsLen = 1;
    grammar[idx].rhs[0] = T(TK_EPS);
    return idx;
}

/* ---- Load all grammar rules ---- */
static void loadGrammarRules(void) {
    GrammarSymbol rhs[MAX_RHS];

    /* Rule 0: <program> -> <otherFunctions> <mainFunction> */
    rhs[0] = NT(NT_otherFunctions); rhs[1] = NT(NT_mainFunction);
    addRuleArr(NT_program, rhs, 2);

    /* Rule 1: <mainFunction> -> TK_MAIN <stmts> TK_END */
    rhs[0] = T(TK_MAIN); rhs[1] = NT(NT_stmts); rhs[2] = T(TK_END);
    addRuleArr(NT_mainFunction, rhs, 3);

    /* Rule 2: <otherFunctions> -> <function> <otherFunctions> */
    rhs[0] = NT(NT_function); rhs[1] = NT(NT_otherFunctions);
    addRuleArr(NT_otherFunctions, rhs, 2);

    /* Rule 3: <otherFunctions> -> eps */
    addEpsRule(NT_otherFunctions);

    /* Rule 4: <function> -> TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END */
    rhs[0] = T(TK_FUNID); rhs[1] = NT(NT_input_par); rhs[2] = NT(NT_output_par);
    rhs[3] = T(TK_SEM); rhs[4] = NT(NT_stmts); rhs[5] = T(TK_END);
    addRuleArr(NT_function, rhs, 6);

    /* Rule 5: <input_par> -> TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR */
    rhs[0] = T(TK_INPUT); rhs[1] = T(TK_PARAMETER); rhs[2] = T(TK_LIST);
    rhs[3] = T(TK_SQL); rhs[4] = NT(NT_parameter_list); rhs[5] = T(TK_SQR);
    addRuleArr(NT_input_par, rhs, 6);

    /* Rule 6: <output_par> -> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR */
    rhs[0] = T(TK_OUTPUT); rhs[1] = T(TK_PARAMETER); rhs[2] = T(TK_LIST);
    rhs[3] = T(TK_SQL); rhs[4] = NT(NT_parameter_list); rhs[5] = T(TK_SQR);
    addRuleArr(NT_output_par, rhs, 6);

    /* Rule 7: <output_par> -> eps */
    addEpsRule(NT_output_par);

    /* Rule 8: <parameter_list> -> <dataType> TK_ID <remaining_list> */
    rhs[0] = NT(NT_dataType); rhs[1] = T(TK_ID); rhs[2] = NT(NT_remaining_list);
    addRuleArr(NT_parameter_list, rhs, 3);

    /* Rule 9: <dataType> -> <primitiveDatatype> */
    rhs[0] = NT(NT_primitiveDatatype);
    addRuleArr(NT_dataType, rhs, 1);

    /* Rule 10: <dataType> -> <constructedDatatype> */
    rhs[0] = NT(NT_constructedDatatype);
    addRuleArr(NT_dataType, rhs, 1);

    /* Rule 11: <primitiveDatatype> -> TK_INT */
    rhs[0] = T(TK_INT);
    addRuleArr(NT_primitiveDatatype, rhs, 1);

    /* Rule 12: <primitiveDatatype> -> TK_REAL */
    rhs[0] = T(TK_REAL);
    addRuleArr(NT_primitiveDatatype, rhs, 1);

    /* Rule 13: <constructedDatatype> -> TK_RECORD TK_RUID */
    rhs[0] = T(TK_RECORD); rhs[1] = T(TK_RUID);
    addRuleArr(NT_constructedDatatype, rhs, 2);

    /* Rule 14: <constructedDatatype> -> TK_UNION TK_RUID */
    rhs[0] = T(TK_UNION); rhs[1] = T(TK_RUID);
    addRuleArr(NT_constructedDatatype, rhs, 2);

    /* Rule 15: <constructedDatatype> -> TK_RUID */
    rhs[0] = T(TK_RUID);
    addRuleArr(NT_constructedDatatype, rhs, 1);

    /* Rule 16: <remaining_list> -> TK_COMMA <parameter_list> */
    rhs[0] = T(TK_COMMA); rhs[1] = NT(NT_parameter_list);
    addRuleArr(NT_remaining_list, rhs, 2);

    /* Rule 17: <remaining_list> -> eps */
    addEpsRule(NT_remaining_list);

    /* Rule 18: <stmts> -> <typeDefinitions> <declarations> <otherStmts> <returnStmt> */
    rhs[0] = NT(NT_typeDefinitions); rhs[1] = NT(NT_declarations);
    rhs[2] = NT(NT_otherStmts); rhs[3] = NT(NT_returnStmt);
    addRuleArr(NT_stmts, rhs, 4);

    /* Rule 19: <typeDefinitions> -> <actualOrRedefined> <typeDefinitions> */
    rhs[0] = NT(NT_actualOrRedefined); rhs[1] = NT(NT_typeDefinitions);
    addRuleArr(NT_typeDefinitions, rhs, 2);

    /* Rule 20: <typeDefinitions> -> eps */
    addEpsRule(NT_typeDefinitions);

    /* Rule 21: <actualOrRedefined> -> <typeDefinition> */
    rhs[0] = NT(NT_typeDefinition);
    addRuleArr(NT_actualOrRedefined, rhs, 1);

    /* Rule 22: <actualOrRedefined> -> <definetypestmt> */
    rhs[0] = NT(NT_definetypestmt);
    addRuleArr(NT_actualOrRedefined, rhs, 1);

    /* Rule 23: <typeDefinition> -> TK_RECORD TK_RUID <fieldDefinitions> TK_ENDRECORD */
    rhs[0] = T(TK_RECORD); rhs[1] = T(TK_RUID); rhs[2] = NT(NT_fieldDefinitions); rhs[3] = T(TK_ENDRECORD);
    addRuleArr(NT_typeDefinition, rhs, 4);

    /* Rule 24: <typeDefinition> -> TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION */
    rhs[0] = T(TK_UNION); rhs[1] = T(TK_RUID); rhs[2] = NT(NT_fieldDefinitions); rhs[3] = T(TK_ENDUNION);
    addRuleArr(NT_typeDefinition, rhs, 4);

    /* Rule 25: <fieldDefinitions> -> <fieldDefinition> <fieldDefinition> <moreFields> */
    rhs[0] = NT(NT_fieldDefinition); rhs[1] = NT(NT_fieldDefinition); rhs[2] = NT(NT_moreFields);
    addRuleArr(NT_fieldDefinitions, rhs, 3);

    /* Rule 26: <fieldDefinition> -> TK_TYPE <fieldType> TK_COLON TK_FIELDID TK_SEM */
    rhs[0] = T(TK_TYPE); rhs[1] = NT(NT_fieldType); rhs[2] = T(TK_COLON);
    rhs[3] = T(TK_ID); rhs[4] = T(TK_SEM);
    addRuleArr(NT_fieldDefinition, rhs, 5);

    /* Rule 27: <fieldType> -> <primitiveDatatype> */
    rhs[0] = NT(NT_primitiveDatatype);
    addRuleArr(NT_fieldType, rhs, 1);

    /* Rule 28: <fieldType> -> <constructedDatatype> */
    rhs[0] = NT(NT_constructedDatatype);
    addRuleArr(NT_fieldType, rhs, 1);

    /* Rule 29: <moreFields> -> <fieldDefinition> <moreFields> */
    rhs[0] = NT(NT_fieldDefinition); rhs[1] = NT(NT_moreFields);
    addRuleArr(NT_moreFields, rhs, 2);

    /* Rule 30: <moreFields> -> eps */
    addEpsRule(NT_moreFields);

    /* Rule 31: <declarations> -> <declaration> <declarations> */
    rhs[0] = NT(NT_declaration); rhs[1] = NT(NT_declarations);
    addRuleArr(NT_declarations, rhs, 2);

    /* Rule 32: <declarations> -> eps */
    addEpsRule(NT_declarations);

    /* Rule 33: <declaration> -> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM */
    rhs[0] = T(TK_TYPE); rhs[1] = NT(NT_dataType); rhs[2] = T(TK_COLON);
    rhs[3] = T(TK_ID); rhs[4] = NT(NT_global_or_not); rhs[5] = T(TK_SEM);
    addRuleArr(NT_declaration, rhs, 6);

    /* Rule 34: <global_or_not> -> TK_COLON TK_GLOBAL */
    rhs[0] = T(TK_COLON); rhs[1] = T(TK_GLOBAL);
    addRuleArr(NT_global_or_not, rhs, 2);

    /* Rule 35: <global_or_not> -> eps */
    addEpsRule(NT_global_or_not);

    /* Rule 36: <otherStmts> -> <stmt> <otherStmts> */
    rhs[0] = NT(NT_stmt); rhs[1] = NT(NT_otherStmts);
    addRuleArr(NT_otherStmts, rhs, 2);

    /* Rule 37: <otherStmts> -> eps */
    addEpsRule(NT_otherStmts);

    /* Rule 38: <stmt> -> <assignmentStmt> */
    rhs[0] = NT(NT_assignmentStmt);
    addRuleArr(NT_stmt, rhs, 1);

    /* Rule 39: <stmt> -> <iterativeStmt> */
    rhs[0] = NT(NT_iterativeStmt);
    addRuleArr(NT_stmt, rhs, 1);

    /* Rule 40: <stmt> -> <conditionalStmt> */
    rhs[0] = NT(NT_conditionalStmt);
    addRuleArr(NT_stmt, rhs, 1);

    /* Rule 41: <stmt> -> <ioStmt> */
    rhs[0] = NT(NT_ioStmt);
    addRuleArr(NT_stmt, rhs, 1);

    /* Rule 42: <stmt> -> <funCallStmt> */
    rhs[0] = NT(NT_funCallStmt);
    addRuleArr(NT_stmt, rhs, 1);

    /* Rule 43: <assignmentStmt> -> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM */
    rhs[0] = NT(NT_singleOrRecId); rhs[1] = T(TK_ASSIGNOP);
    rhs[2] = NT(NT_arithmeticExpression); rhs[3] = T(TK_SEM);
    addRuleArr(NT_assignmentStmt, rhs, 4);

    /* Rule 44: <singleOrRecId> -> TK_ID <option_single_constructed> */
    rhs[0] = T(TK_ID); rhs[1] = NT(NT_option_single_constructed);
    addRuleArr(NT_singleOrRecId, rhs, 2);

    /* Rule 45: <option_single_constructed> -> eps */
    addEpsRule(NT_option_single_constructed);

    /* Rule 46: <option_single_constructed> -> <oneExpansion> <moreExpansions> */
    rhs[0] = NT(NT_oneExpansion); rhs[1] = NT(NT_moreExpansions);
    addRuleArr(NT_option_single_constructed, rhs, 2);

    /* Rule 47: <oneExpansion> -> TK_DOT TK_FIELDID */
    rhs[0] = T(TK_DOT); rhs[1] = T(TK_ID);
    addRuleArr(NT_oneExpansion, rhs, 2);

    /* Rule 48: <moreExpansions> -> <oneExpansion> <moreExpansions> */
    rhs[0] = NT(NT_oneExpansion); rhs[1] = NT(NT_moreExpansions);
    addRuleArr(NT_moreExpansions, rhs, 2);

    /* Rule 49: <moreExpansions> -> eps */
    addEpsRule(NT_moreExpansions);

    /* Rule 50: <funCallStmt> -> <outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM */
    rhs[0] = NT(NT_outputParameters); rhs[1] = T(TK_CALL); rhs[2] = T(TK_FUNID);
    rhs[3] = T(TK_WITH); rhs[4] = T(TK_PARAMETERS); rhs[5] = NT(NT_inputParameters);
    rhs[6] = T(TK_SEM);
    addRuleArr(NT_funCallStmt, rhs, 7);

    /* Rule 51: <outputParameters> -> TK_SQL <idList> TK_SQR TK_ASSIGNOP */
    rhs[0] = T(TK_SQL); rhs[1] = NT(NT_idList); rhs[2] = T(TK_SQR); rhs[3] = T(TK_ASSIGNOP);
    addRuleArr(NT_outputParameters, rhs, 4);

    /* Rule 52: <outputParameters> -> eps */
    addEpsRule(NT_outputParameters);

    /* Rule 53: <inputParameters> -> TK_SQL <idList> TK_SQR */
    rhs[0] = T(TK_SQL); rhs[1] = NT(NT_idList); rhs[2] = T(TK_SQR);
    addRuleArr(NT_inputParameters, rhs, 3);

    /* Rule 54: <iterativeStmt> -> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt> <otherStmts> TK_ENDWHILE */
    rhs[0] = T(TK_WHILE); rhs[1] = T(TK_OP); rhs[2] = NT(NT_booleanExpression);
    rhs[3] = T(TK_CL); rhs[4] = NT(NT_stmt); rhs[5] = NT(NT_otherStmts); rhs[6] = T(TK_ENDWHILE);
    addRuleArr(NT_iterativeStmt, rhs, 7);

    /* Rule 55: <conditionalStmt> -> TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt> <otherStmts> <elsePart> */
    rhs[0] = T(TK_IF); rhs[1] = T(TK_OP); rhs[2] = NT(NT_booleanExpression);
    rhs[3] = T(TK_CL); rhs[4] = T(TK_THEN); rhs[5] = NT(NT_stmt);
    rhs[6] = NT(NT_otherStmts); rhs[7] = NT(NT_elsePart);
    addRuleArr(NT_conditionalStmt, rhs, 8);

    /* Rule 56: <elsePart> -> TK_ELSE <stmt> <otherStmts> TK_ENDIF */
    rhs[0] = T(TK_ELSE); rhs[1] = NT(NT_stmt); rhs[2] = NT(NT_otherStmts); rhs[3] = T(TK_ENDIF);
    addRuleArr(NT_elsePart, rhs, 4);

    /* Rule 57: <elsePart> -> TK_ENDIF */
    rhs[0] = T(TK_ENDIF);
    addRuleArr(NT_elsePart, rhs, 1);

    /* Rule 58: <ioStmt> -> TK_READ TK_OP <var> TK_CL TK_SEM */
    rhs[0] = T(TK_READ); rhs[1] = T(TK_OP); rhs[2] = NT(NT_var);
    rhs[3] = T(TK_CL); rhs[4] = T(TK_SEM);
    addRuleArr(NT_ioStmt, rhs, 5);

    /* Rule 59: <ioStmt> -> TK_WRITE TK_OP <var> TK_CL TK_SEM */
    rhs[0] = T(TK_WRITE); rhs[1] = T(TK_OP); rhs[2] = NT(NT_var);
    rhs[3] = T(TK_CL); rhs[4] = T(TK_SEM);
    addRuleArr(NT_ioStmt, rhs, 5);

    /* Rule 60: <arithmeticExpression> -> <term> <expPrime> (B1) */
    rhs[0] = NT(NT_term); rhs[1] = NT(NT_expPrime);
    addRuleArr(NT_arithmeticExpression, rhs, 2);

    /* Rule 61: <expPrime> -> <lowPrecedenceOperators> <term> <expPrime> (B2) */
    rhs[0] = NT(NT_lowPrecedenceOperators); rhs[1] = NT(NT_term); rhs[2] = NT(NT_expPrime);
    addRuleArr(NT_expPrime, rhs, 3);

    /* Rule 62: <expPrime> -> eps (B2) */
    addEpsRule(NT_expPrime);

    /* Rule 63: <term> -> <factor> <termPrime> (B3) */
    rhs[0] = NT(NT_factor); rhs[1] = NT(NT_termPrime);
    addRuleArr(NT_term, rhs, 2);

    /* Rule 64: <termPrime> -> <highPrecedenceOperators> <factor> <termPrime> (B4) */
    rhs[0] = NT(NT_highPrecedenceOperators); rhs[1] = NT(NT_factor); rhs[2] = NT(NT_termPrime);
    addRuleArr(NT_termPrime, rhs, 3);

    /* Rule 65: <termPrime> -> eps (B4) */
    addEpsRule(NT_termPrime);

    /* Rule 66: <factor> -> TK_OP <arithmeticExpression> TK_CL (B5) */
    rhs[0] = T(TK_OP); rhs[1] = NT(NT_arithmeticExpression); rhs[2] = T(TK_CL);
    addRuleArr(NT_factor, rhs, 3);

    /* Rule 67: <factor> -> <var> (B5) */
    rhs[0] = NT(NT_var);
    addRuleArr(NT_factor, rhs, 1);

    /* Rule 68: <highPrecedenceOperators> -> TK_MUL (B6) */
    rhs[0] = T(TK_MUL);
    addRuleArr(NT_highPrecedenceOperators, rhs, 1);

    /* Rule 69: <highPrecedenceOperators> -> TK_DIV (B6) */
    rhs[0] = T(TK_DIV);
    addRuleArr(NT_highPrecedenceOperators, rhs, 1);

    /* Rule 70: <lowPrecedenceOperators> -> TK_PLUS (B7) */
    rhs[0] = T(TK_PLUS);
    addRuleArr(NT_lowPrecedenceOperators, rhs, 1);

    /* Rule 71: <lowPrecedenceOperators> -> TK_MINUS (B7) */
    rhs[0] = T(TK_MINUS);
    addRuleArr(NT_lowPrecedenceOperators, rhs, 1);

    /* Rule 72: <booleanExpression> -> TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL */
    rhs[0] = T(TK_OP); rhs[1] = NT(NT_booleanExpression); rhs[2] = T(TK_CL);
    rhs[3] = NT(NT_logicalOp); rhs[4] = T(TK_OP); rhs[5] = NT(NT_booleanExpression); rhs[6] = T(TK_CL);
    addRuleArr(NT_booleanExpression, rhs, 7);

    /* Rule 73: <booleanExpression> -> <var> <relationalOp> <var> */
    rhs[0] = NT(NT_var); rhs[1] = NT(NT_relationalOp); rhs[2] = NT(NT_var);
    addRuleArr(NT_booleanExpression, rhs, 3);

    /* Rule 74: <booleanExpression> -> TK_NOT TK_OP <booleanExpression> TK_CL */
    rhs[0] = T(TK_NOT); rhs[1] = T(TK_OP); rhs[2] = NT(NT_booleanExpression); rhs[3] = T(TK_CL);
    addRuleArr(NT_booleanExpression, rhs, 4);

    /* Rule 75: <var> -> <singleOrRecId> */
    rhs[0] = NT(NT_singleOrRecId);
    addRuleArr(NT_var, rhs, 1);

    /* Rule 76: <var> -> TK_NUM */
    rhs[0] = T(TK_NUM);
    addRuleArr(NT_var, rhs, 1);

    /* Rule 77: <var> -> TK_RNUM */
    rhs[0] = T(TK_RNUM);
    addRuleArr(NT_var, rhs, 1);

    /* Rule 78: <logicalOp> -> TK_AND */
    rhs[0] = T(TK_AND);
    addRuleArr(NT_logicalOp, rhs, 1);

    /* Rule 79: <logicalOp> -> TK_OR */
    rhs[0] = T(TK_OR);
    addRuleArr(NT_logicalOp, rhs, 1);

    /* Rule 80: <relationalOp> -> TK_LT */
    rhs[0] = T(TK_LT);
    addRuleArr(NT_relationalOp, rhs, 1);

    /* Rule 81: <relationalOp> -> TK_LE */
    rhs[0] = T(TK_LE);
    addRuleArr(NT_relationalOp, rhs, 1);

    /* Rule 82: <relationalOp> -> TK_EQ */
    rhs[0] = T(TK_EQ);
    addRuleArr(NT_relationalOp, rhs, 1);

    /* Rule 83: <relationalOp> -> TK_GT */
    rhs[0] = T(TK_GT);
    addRuleArr(NT_relationalOp, rhs, 1);

    /* Rule 84: <relationalOp> -> TK_GE */
    rhs[0] = T(TK_GE);
    addRuleArr(NT_relationalOp, rhs, 1);

    /* Rule 85: <relationalOp> -> TK_NE */
    rhs[0] = T(TK_NE);
    addRuleArr(NT_relationalOp, rhs, 1);

    /* Rule 86: <returnStmt> -> TK_RETURN <optionalReturn> TK_SEM */
    rhs[0] = T(TK_RETURN); rhs[1] = NT(NT_optionalReturn); rhs[2] = T(TK_SEM);
    addRuleArr(NT_returnStmt, rhs, 3);

    /* Rule 87: <optionalReturn> -> TK_SQL <idList> TK_SQR */
    rhs[0] = T(TK_SQL); rhs[1] = NT(NT_idList); rhs[2] = T(TK_SQR);
    addRuleArr(NT_optionalReturn, rhs, 3);

    /* Rule 88: <optionalReturn> -> eps */
    addEpsRule(NT_optionalReturn);

    /* Rule 89: <idList> -> TK_ID <more_ids> */
    rhs[0] = T(TK_ID); rhs[1] = NT(NT_more_ids);
    addRuleArr(NT_idList, rhs, 2);

    /* Rule 90: <more_ids> -> TK_COMMA <idList> */
    rhs[0] = T(TK_COMMA); rhs[1] = NT(NT_idList);
    addRuleArr(NT_more_ids, rhs, 2);

    /* Rule 91: <more_ids> -> eps */
    addEpsRule(NT_more_ids);

    /* Rule 92: <definetypestmt> -> TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID */
    rhs[0] = T(TK_DEFINETYPE); rhs[1] = NT(NT_A); rhs[2] = T(TK_RUID);
    rhs[3] = T(TK_AS); rhs[4] = T(TK_RUID);
    addRuleArr(NT_definetypestmt, rhs, 5);

    /* Rule 93: <A> -> TK_RECORD */
    rhs[0] = T(TK_RECORD);
    addRuleArr(NT_A, rhs, 1);

    /* Rule 94: <A> -> TK_UNION */
    rhs[0] = T(TK_UNION);
    addRuleArr(NT_A, rhs, 1);
}

/* ---- Compute FIRST sets (fixed-point iteration) ---- */
static void computeFirstSets(void) {
    memset(firstSet, 0, sizeof(firstSet));
    memset(nullable, 0, sizeof(nullable));

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int r = 0; r < numRules; r++) {
            NonTerminalType lhs = grammar[r].lhs;
            int rhsLen = grammar[r].rhsLen;

            /* Check if rule is epsilon */
            if (rhsLen == 1 && grammar[r].rhs[0].isTerminal && grammar[r].rhs[0].symbol == TK_EPS) {
                if (!nullable[lhs]) {
                    nullable[lhs] = 1;
                    changed = 1;
                }
                continue;
            }

            /* For each symbol in RHS, add its FIRST to LHS's FIRST */
            int allNullable = 1;
            for (int i = 0; i < rhsLen; i++) {
                GrammarSymbol sym = grammar[r].rhs[i];
                if (sym.isTerminal) {
                    if (sym.symbol != TK_EPS) {
                        if (!firstSet[lhs][sym.symbol]) {
                            firstSet[lhs][sym.symbol] = 1;
                            changed = 1;
                        }
                    }
                    allNullable = 0;
                    break;
                } else {
                    /* Nonterminal: add its FIRST set */
                    for (int t = 0; t < NUM_TERMINALS; t++) {
                        if (firstSet[sym.symbol][t] && !firstSet[lhs][t]) {
                            firstSet[lhs][t] = 1;
                            changed = 1;
                        }
                    }
                    if (!nullable[sym.symbol]) {
                        allNullable = 0;
                        break;
                    }
                }
            }
            if (allNullable && !nullable[lhs]) {
                nullable[lhs] = 1;
                changed = 1;
            }
        }
    }
}

/* ---- Compute FOLLOW sets (fixed-point iteration) ---- */
static void computeFollowSets(void) {
    memset(followSet, 0, sizeof(followSet));

    /* Start symbol: add $ (TK_EOF) to FOLLOW */
    followSet[NT_program][TK_EOF] = 1;

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int r = 0; r < numRules; r++) {
            NonTerminalType lhs = grammar[r].lhs;
            int rhsLen = grammar[r].rhsLen;

            /* Skip epsilon rules */
            if (rhsLen == 1 && grammar[r].rhs[0].isTerminal && grammar[r].rhs[0].symbol == TK_EPS)
                continue;

            for (int i = 0; i < rhsLen; i++) {
                GrammarSymbol sym = grammar[r].rhs[i];
                if (sym.isTerminal) continue;

                /* sym is a nonterminal B at position i */
                NonTerminalType B = (NonTerminalType)sym.symbol;

                /* Add FIRST of everything after B (beta) to FOLLOW(B) */
                int betaAllNullable = 1;
                for (int j = i + 1; j < rhsLen; j++) {
                    GrammarSymbol next = grammar[r].rhs[j];
                    if (next.isTerminal) {
                        if (next.symbol != TK_EPS && !followSet[B][next.symbol]) {
                            followSet[B][next.symbol] = 1;
                            changed = 1;
                        }
                        betaAllNullable = 0;
                        break;
                    } else {
                        /* Add FIRST of this nonterminal */
                        for (int t = 0; t < NUM_TERMINALS; t++) {
                            if (firstSet[next.symbol][t] && !followSet[B][t]) {
                                followSet[B][t] = 1;
                                changed = 1;
                            }
                        }
                        if (!nullable[next.symbol]) {
                            betaAllNullable = 0;
                            break;
                        }
                    }
                }

                /* If beta derives epsilon (or B is last), add FOLLOW(LHS) to FOLLOW(B) */
                if (betaAllNullable) {
                    for (int t = 0; t < NUM_TERMINALS; t++) {
                        if (followSet[lhs][t] && !followSet[B][t]) {
                            followSet[B][t] = 1;
                            changed = 1;
                        }
                    }
                }
            }
        }
    }
}

/* ---- Compute FIRST of a particular RHS string ---- */
static void firstOfRHS(GrammarSymbol *rhs, int len, int result[NUM_TERMINALS], int *canBeEps) {
    memset(result, 0, sizeof(int) * NUM_TERMINALS);
    *canBeEps = 1;

    if (len == 1 && rhs[0].isTerminal && rhs[0].symbol == TK_EPS) {
        *canBeEps = 1;
        return;
    }

    for (int i = 0; i < len; i++) {
        if (rhs[i].isTerminal) {
            if (rhs[i].symbol != TK_EPS)
                result[rhs[i].symbol] = 1;
            *canBeEps = 0;
            return;
        } else {
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (firstSet[rhs[i].symbol][t])
                    result[t] = 1;
            }
            if (!nullable[rhs[i].symbol]) {
                *canBeEps = 0;
                return;
            }
        }
    }
    /* If we get here, everything was nullable */
}

/* ---- Build parse table ---- */
static void buildParseTable(void) {
    /* Initialize to -1 (error) */
    for (int i = 0; i < NUM_NONTERMINALS; i++)
        for (int j = 0; j < NUM_TERMINALS; j++)
            parseTable[i][j] = -1;

    for (int r = 0; r < numRules; r++) {
        NonTerminalType lhs = grammar[r].lhs;
        int firstOfRHSArr[NUM_TERMINALS];
        int canBeEps;

        firstOfRHS(grammar[r].rhs, grammar[r].rhsLen, firstOfRHSArr, &canBeEps);

        /* For each terminal in FIRST(rhs), add rule */
        for (int t = 0; t < NUM_TERMINALS; t++) {
            if (t == TK_EPS || t == TK_EOF || t == TK_ERROR) continue;
            if (firstOfRHSArr[t]) {
                parseTable[lhs][t] = r;
            }
        }

        /* If RHS can derive epsilon, add rule for each terminal in FOLLOW(lhs) */
        if (canBeEps) {
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (t == TK_EPS) continue;
                if (followSet[lhs][t]) {
                    if (parseTable[lhs][t] == -1)
                        parseTable[lhs][t] = r;
                }
            }
        }
    }
}

/* ---- Public interface ---- */
void initGrammar(void) {
    numRules = 0;
    loadGrammarRules();
    computeFirstSets();
    computeFollowSets();
    buildParseTable();
}

void printFirstFollowSets(void) {
    printf("\n=== FIRST SETS ===\n");
    for (int nt = 0; nt < NUM_NONTERMINALS; nt++) {
        printf("FIRST(%-30s) = { ", getNonTerminalName((NonTerminalType)nt));
        int first = 1;
        if (nullable[nt]) {
            printf("eps");
            first = 0;
        }
        for (int t = 0; t < NUM_TERMINALS; t++) {
            if (firstSet[nt][t]) {
                if (!first) printf(", ");
                printf("%s", getTerminalName((TokenType)t));
                first = 0;
            }
        }
        printf(" }\n");
    }

    printf("\n=== FOLLOW SETS ===\n");
    for (int nt = 0; nt < NUM_NONTERMINALS; nt++) {
        printf("FOLLOW(%-30s) = { ", getNonTerminalName((NonTerminalType)nt));
        int first = 1;
        for (int t = 0; t < NUM_TERMINALS; t++) {
            if (followSet[nt][t]) {
                if (!first) printf(", ");
                printf("%s", getTerminalName((TokenType)t));
                first = 0;
            }
        }
        printf(" }\n");
    }
}
