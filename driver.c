/*
 * driver.c - Main driver for the compiler front-end (Stage 1)
 * CS F363 - Compiler Construction
 *
 * Group: CoCoMelon
 *
 * Usage: ./stage1exe testcase.txt parsetreeOutFile.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "token.h"
#include "lexer.h"
#include "grammar.h"
#include "parser.h"

static void printBanner(void) {
    printf("============================================================\n");
    printf("  Compiler Front-End - Stage 1 (Lexer + Parser)\n");
    printf("  CS F363 - Compiler Construction\n");
    printf("============================================================\n");
    printf("Implementation Status:\n");
    printf("  (a) FIRST and FOLLOW set automated\n");
    printf("  (b) Both lexical and syntax analysis modules implemented\n");
    printf("  (c) Parse tree constructed via inorder traversal\n");
    printf("============================================================\n\n");
}

static void printMenu(void) {
    printf("\nSelect an option:\n");
    printf("  0 : Exit\n");
    printf("  1 : Remove comments (print comment-free code)\n");
    printf("  2 : Print token list (lexical analysis only)\n");
    printf("  3 : Parse and print parse tree (lexer + parser)\n");
    printf("  4 : Print total time taken (lexer + parser)\n");
    printf("Enter option: ");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <sourcefile> <parsetreeOutFile>\n", argv[0]);
        return 1;
    }

    const char *sourceFile = argv[1];
    const char *parseTreeFile = argv[2];

    /* Initialize grammar once (FIRST/FOLLOW/parse table) */
    initGrammar();

    printBanner();

    int option = -1;
    while (option != 0) {
        printMenu();
        if (scanf("%d", &option) != 1) {
            /* Clear invalid input */
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (option) {
            case 0:
                printf("Exiting.\n");
                break;

            case 1:
                /* Option 1: Remove comments */
                printf("\n--- Comment-free source code ---\n");
                printCommentFreeCode(sourceFile);
                printf("\n--- End ---\n");
                break;

            case 2:
                /* Option 2: Print token list */
                printf("\n--- Token List ---\n");
                printAllTokens(sourceFile);
                printf("--- End ---\n");
                break;

            case 3: {
                /* Option 3: Parse and output parse tree */
                initLexer(sourceFile);
                ParseTreeNode *tree = parseInputSource();
                if (tree) {
                    FILE *outFp = fopen(parseTreeFile, "w");
                    if (outFp) {
                        printParseTree(tree, outFp);
                        fclose(outFp);
                        printf("Parse tree written to '%s'\n", parseTreeFile);
                    } else {
                        fprintf(stderr, "Error: Cannot open output file '%s'\n", parseTreeFile);
                    }
                    freeParseTree(tree);
                }
                cleanupLexer();
                break;
            }

            case 4: {
                /* Option 4: Time the lexer + parser */
                clock_t start_time, end_time;
                double total_CPU_time, total_CPU_time_in_seconds;

                start_time = clock();

                initLexer(sourceFile);
                ParseTreeNode *tree = parseInputSource();
                if (tree) {
                    freeParseTree(tree);
                }
                cleanupLexer();

                end_time = clock();

                total_CPU_time = (double)(end_time - start_time);
                total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

                printf("\nTotal CPU time (clock ticks): %f\n", total_CPU_time);
                printf("Total CPU time (seconds):     %f\n", total_CPU_time_in_seconds);
                break;
            }

            default:
                printf("Invalid option. Please select 0-4.\n");
                break;
        }
    }

    return 0;
}
