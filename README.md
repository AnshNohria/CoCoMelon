# CoCoMelon — Compiler Front-End (Stage 1)

**CS F363 — Compiler Construction | BITS Pilani**

A complete Stage 1 compiler front-end implementing a **Lexical Analyzer (Lexer)** and an **LL(1) Predictive Parser** for a custom language specification. The system tokenizes source code, constructs a parse tree via inorder traversal, and reports lexical/syntactic errors with panic-mode recovery.

---

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [File Descriptions](#file-descriptions)
- [Prerequisites](#prerequisites)
- [Building the Project](#building-the-project)
- [Running the Compiler](#running-the-compiler)
  - [Menu Options](#menu-options)
  - [Examples](#examples)
- [Token Specification](#token-specification)
  - [Identifier Patterns](#identifier-patterns)
  - [Numeric Literals](#numeric-literals)
  - [Operators](#operators)
  - [Keywords](#keywords)
- [Grammar](#grammar)
- [Error Handling](#error-handling)
- [Parse Tree Output Format](#parse-tree-output-format)
- [Test Cases](#test-cases)

---

## Features

- **Automated FIRST and FOLLOW set computation** via fixed-point iteration
- **LL(1) parse table construction** from 83+ grammar productions
- **DFA-based lexical analysis** with 31 keywords and 5 identifier types
- **Parse tree construction** with inorder traversal output
- **Panic-mode error recovery** using FOLLOW sets to reduce cascading errors
- **Comment removal** (prints clean source code)
- **Performance timing** (CPU clock ticks and seconds)
- **Lexical error reporting** — unknown symbols, invalid patterns, overlength identifiers
- **Syntax error reporting** — expected vs. found tokens with line numbers

---

## Project Structure

```
CoCoMelon/
├── token.h                # Token types, nonterminal enums, parse tree node structs
├── lexer.h                # Lexer public interface
├── lexer.c                # DFA-based lexical analyzer implementation
├── grammar.h              # Grammar rules, FIRST/FOLLOW, parse table declarations
├── grammar.c              # 83+ grammar rules, automated FIRST/FOLLOW/parse table
├── parser.h               # Parser public interface
├── parser.c               # LL(1) predictive parser with parse tree construction
├── driver.c               # Main entry point with interactive menu
├── makefile               # Build configuration (GCC)
├── Specification.md       # Language specification and project requirements
├── Plan.md                # Implementation plan and design notes
├── Lexer Test Cases/      # Official lexer test inputs and expected outputs
│   └── Lexer Test Cases/
│       ├── t1.txt                    # Lexer test input 1
│       ├── t2.txt                    # Lexer test input 2
│       ├── lexemesandtokens_t1.txt   # Expected token output for t1
│       └── lexemesandtokens_t2.txt   # Expected token output for t2
├── Parser Test Cases/     # Official parser test inputs
│   ├── t3.txt             # Syntactically correct program
│   ├── t4.txt             # Syntactically correct program (with boolean ops)
│   ├── t5.txt             # Syntactically correct program
│   ├── t6.txt             # Program with intentional syntax errors
│   └── listoferrors_t6.txt # Expected errors for t6
└── README.md              # This file
```

---

## File Descriptions

### Header Files

| File | Purpose |
|------|---------|
| **`token.h`** | Central type definitions. Defines the `TokenType` enum (60 token types including keywords, operators, identifiers, `TK_EOF`, `TK_ERROR`, `TK_COMMENT`, `TK_EPS`), the `NonTerminalType` enum (53 nonterminals), the `Token` struct (type, lexeme, line number), and the `ParseTreeNode` struct (tree node with parent/children pointers, rule number). Also provides `getTerminalName()` and `getNonTerminalName()` utility functions. |
| **`lexer.h`** | Public interface for the lexer module. Declares `initLexer()`, `cleanupLexer()`, `resetLexer()`, `getNextToken()`, `printCommentFreeCode()`, and `printAllTokens()`. |
| **`grammar.h`** | Public interface for the grammar module. Declares the `GrammarSymbol` and `Rule` structs, the grammar array, parse table, FOLLOW sets, and the `initGrammar()` and `printFirstFollowSets()` functions. Constants: `MAX_RHS = 10`, `MAX_RULES = 120`. |
| **`parser.h`** | Public interface for the parser module. Declares `parseInputSource()` (returns parse tree root), `printParseTree()` (inorder output to file), and `freeParseTree()`. |

### Source Files

| File | Purpose |
|------|---------|
| **`lexer.c`** | Implements the lexical analyzer using a hand-coded DFA. Reads the entire source file into a buffer, then scans character-by-character to produce tokens. Handles identifier disambiguation (TK_ID vs TK_FIELDID vs keywords), numeric literals with scientific notation, multi-character operators (`<---`, `&&&`, `@@@`), comments (`%`), and error reporting for invalid patterns and overlength identifiers. |
| **`grammar.c`** | Stores all 83+ grammar production rules as structured data. Implements automated FIRST set computation (fixed-point iteration with nullable tracking), FOLLOW set computation, and LL(1) parse table construction. Detects and reports parse table conflicts. |
| **`parser.c`** | Implements a stack-based LL(1) predictive parser. Maintains a parsing stack of grammar symbols, looks up the parse table for expansion rules, and constructs a full parse tree during parsing. Uses `getNextParserToken()` to automatically skip `TK_COMMENT` and `TK_ERROR` tokens. Implements panic-mode error recovery using FOLLOW sets. Outputs the parse tree via inorder traversal. |
| **`driver.c`** | Main program entry point. Parses command-line arguments (source file + output file), displays a banner, and runs an interactive menu loop with 5 options (0–4). Each option independently initializes the lexer/parser as needed. |
| **`makefile`** | Build automation. Compiles each `.c` file to `.o` with `gcc -Wall -Wextra -g`, then links into `stage1exe`. Provides `make clean` to remove build artifacts. |

### Dependency Graph

```
driver.c ──→ token.h, lexer.h, grammar.h, parser.h
lexer.c  ──→ token.h, lexer.h
grammar.c ─→ token.h, grammar.h
parser.c ──→ token.h, lexer.h, grammar.h, parser.h
```

---

## Prerequisites

- **GCC** (MinGW on Windows, or standard GCC on Linux/macOS)
- **GNU Make** (`make` on Linux/macOS, `mingw32-make` on Windows with MinGW)

### Linux / macOS

```bash
# GCC and Make are typically pre-installed. Verify:
gcc --version
make --version
```

### Windows (MinGW)

```powershell
# If using MinGW, ensure gcc and mingw32-make are in your PATH
gcc --version
mingw32-make --version
```

---

## Building the Project

### Linux / macOS

```bash
cd CoCoMelon
make clean    # Remove previous build artifacts
make          # Compile and link → produces 'stage1exe'
```

### Windows (MinGW)

```powershell
cd CoCoMelon
mingw32-make clean    # Remove previous build artifacts (may show error if no artifacts exist — safe to ignore)
mingw32-make          # Compile and link → produces 'stage1exe.exe'
```

### What Happens During Build

```
gcc -Wall -Wextra -g -c driver.c  -o driver.o
gcc -Wall -Wextra -g -c lexer.c   -o lexer.o
gcc -Wall -Wextra -g -c grammar.c -o grammar.o
gcc -Wall -Wextra -g -c parser.c  -o parser.o
gcc -Wall -Wextra -g -o stage1exe driver.o lexer.o grammar.o parser.o
```

---

## Running the Compiler

### Usage

```bash
./stage1exe <sourceFile> <parseTreeOutputFile>
```

| Argument | Description |
|----------|-------------|
| `<sourceFile>` | Path to the source code file to compile |
| `<parseTreeOutputFile>` | Path where the parse tree will be written (used by Option 3) |

### Menu Options

After launching, an interactive menu is displayed:

```
Select an option:
  0 : Exit
  1 : Remove comments (print comment-free code)
  2 : Print token list (lexical analysis only)
  3 : Parse and print parse tree (lexer + parser)
  4 : Print total time taken (lexer + parser)
Enter option:
```

| Option | What It Does |
|--------|-------------|
| **0** | Exits the program. |
| **1** | Reads the source file and prints it to the console with all comment lines (starting with `%`) removed. Only the lexer is used. |
| **2** | Tokenizes the entire source file and prints a formatted table of all tokens with their lexeme and line number. Lexical errors are printed to stderr. |
| **3** | Runs the full lexer + parser pipeline. If the source is syntactically correct, prints "Input source code is syntactically correct." and writes the parse tree to the output file. If there are syntax errors, they are reported to stderr and the (partial) parse tree is still written. |
| **4** | Runs the full lexer + parser pipeline and reports the total CPU time taken in both clock ticks and seconds. |

> **Note:** You can select multiple options in a single session. Each option re-initializes the lexer/parser independently.

### Examples

#### Remove Comments (Option 1)

```bash
$ ./stage1exe testcase.txt output.txt
# Select option 1
```

Output:

```
--- Comment-free source code ---

_main
        type int :b3;
        type real :c4bd56;
        b3 <--- 7;
        ...
--- End ---
```

#### Print Token List (Option 2)

```bash
$ ./stage1exe testcase.txt output.txt
# Select option 2
```

Output:

```
--- Token List ---
Token                     Lexeme                    Line No.
-----------------------------------------------------------
TK_COMMENT                %                         1
TK_MAIN                   _main                     3
TK_TYPE                   type                      4
TK_INT                    int                       4
TK_COLON                  :                         4
TK_ID                     b3                        4
TK_SEM                    ;                         4
...
--- End ---
```

#### Parse and Generate Parse Tree (Option 3)

```bash
$ ./stage1exe "Parser Test Cases/t3.txt" parsetree_out.txt
# Select option 3
```

Output:

```
Input source code is syntactically correct.
Parse tree written to 'parsetree_out.txt'
```

#### Measure Performance (Option 4)

```bash
$ ./stage1exe testcase.txt output.txt
# Select option 4
```

Output:

```
Parse tree written to 'output.txt'

Total CPU time: 1234 clock ticks
Total CPU time in seconds: 0.001234 seconds
```

---

## Token Specification

### Identifier Patterns

| Token | Regex Pattern | Description | Max Length | Examples |
|-------|---------------|-------------|-----------|----------|
| `TK_ID` | `[b-d][2-7][b-d]*[2-7]*` | Variable identifier. First char must be b, c, or d. Second char must be 2–7. Then optional b–d letters followed by optional 2–7 digits. | 20 | `b2`, `c3bd`, `d4cbcd5677` |
| `TK_FIELDID` | `[a-z][a-z]*` | Field/record field identifier. Only lowercase letters, no digits. If the lexeme matches a keyword, the keyword token is returned instead. | 20 | `listb`, `endrecorb`, `value` |
| `TK_FUNID` | `_[a-zA-Z0-9_]+` | Function identifier. Starts with underscore. Special case: `_main` returns `TK_MAIN` instead. | 30 | `_sumN`, `_AbC23`, `_computeFunctionValue` |
| `TK_RUID` | `#[a-z]+` | Record/union type identifier. Hash followed by lowercase letters only. | — | `#new`, `#radix` |

### Numeric Literals

| Token | Pattern | Description | Examples |
|-------|---------|-------------|----------|
| `TK_NUM` | `[0-9]+` | Integer literal | `0`, `42`, `5000` |
| `TK_RNUM` | `[0-9]+\.[0-9]{2}(E[+-]?[0-9]{2})?` | Real number. Must have exactly 2 decimal digits. Optional scientific notation with exactly 2 exponent digits. | `2.35`, `0.00`, `34.98E+23`, `343333.55E-12` |

> **Important:** A decimal number with other than exactly 2 digits after the dot (e.g., `5000.7`, `123.5`) is a lexical error.

### Operators

| Token | Symbol | Description |
|-------|--------|-------------|
| `TK_ASSIGNOP` | `<---` | Assignment operator |
| `TK_PLUS` | `+` | Addition |
| `TK_MINUS` | `-` | Subtraction |
| `TK_MUL` | `*` | Multiplication |
| `TK_DIV` | `/` | Division |
| `TK_LT` | `<` | Less than |
| `TK_LE` | `<=` | Less than or equal |
| `TK_EQ` | `==` | Equal |
| `TK_GT` | `>` | Greater than |
| `TK_GE` | `>=` | Greater than or equal |
| `TK_NE` | `!=` | Not equal |
| `TK_AND` | `&&&` | Logical AND |
| `TK_OR` | `@@@` | Logical OR |
| `TK_NOT` | `~` | Logical NOT |

### Punctuation

| Token | Symbol |
|-------|--------|
| `TK_SQL` | `[` |
| `TK_SQR` | `]` |
| `TK_OP` | `(` |
| `TK_CL` | `)` |
| `TK_SEM` | `;` |
| `TK_COLON` | `:` |
| `TK_DOT` | `.` |
| `TK_COMMA` | `,` |

### Keywords

The following 31 words are reserved keywords. They match the `TK_FIELDID` pattern `[a-z]+` but are returned as their respective keyword token:

```
main       end        int        real       record     union      endrecord
endunion   type       global     input      output     parameter  list
definetype as         while      endwhile   if         then       else
endif      read       write      call       with       parameters return
not        and        or
```

### Special Tokens

| Token | Description |
|-------|-------------|
| `TK_COMMENT` | Comment line (starts with `%`). Emitted by lexer, automatically skipped by parser. |
| `TK_EOF` | End of file marker. |
| `TK_ERROR` | Lexical error token (unknown symbols, invalid patterns, overlength identifiers). |
| `TK_EPS` | Epsilon (empty production) — used internally by the grammar. |

---

## Grammar

The language grammar consists of **83+ LL(1) production rules** with **53 nonterminals**.

**Start symbol:** `<program>`

### Sample Productions

```
<program>          → <otherFunctions> <mainFunction>
<mainFunction>     → TK_MAIN <stmts> TK_END
<otherFunctions>   → <function> <otherFunctions> | ε
<function>         → TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
<stmts>            → <typeDefinitions> <declarations> <otherStmts> <returnStmt>
<iterativeStmt>    → TK_WHILE TK_OP <booleanExpression> TK_CL <stmt> <otherStmts> TK_ENDWHILE
<conditionalStmt>  → TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt> <otherStmts>
                     TK_ELSE <stmt> <otherStmts> TK_ENDIF
<ioStmt>           → TK_READ TK_OP <var> TK_CL TK_SEM
                   | TK_WRITE TK_OP <var> TK_CL TK_SEM
```

The FIRST and FOLLOW sets are computed automatically at startup via fixed-point iteration, and the LL(1) parse table is built from these sets.

---

## Error Handling

### Lexical Errors (reported to stderr)

| Error Type | Example | Message Format |
|-----------|---------|----------------|
| Unknown symbol | `$`, `\|` | `Lexical Error at line X: Unknown symbol 'Y'` |
| Invalid pattern | `&&`, `5000.7` | `Lexical Error at line X: Unknown pattern <Y>` |
| Variable ID too long | >20 chars | `Lexical Error at line X: Variable Identifier is longer than the prescribed length of 20 characters.` |
| Function ID too long | >30 chars | `Lexical Error at line X: Function Identifier is longer than the prescribed length of 30 characters.` |
| Invalid assignment | `<--`, `<-` | `Lexical Error at line X: Unknown pattern <Y>` |

### Syntax Errors (reported to stderr)

| Error Type | Message Format |
|-----------|----------------|
| Terminal mismatch | `Syntax Error at line X: Expected 'TK_Y', got 'TK_Z' (lexeme: 'w')` |
| No parse table entry | `Syntax Error at line X: Unexpected token 'TK_Y' (lexeme: 'w') for nonterminal <nt>` |

**Recovery strategy:** Panic-mode using FOLLOW sets. If the current token is in the FOLLOW set of the nonterminal on top of the stack, the nonterminal is popped (treated as epsilon). Otherwise, the current token is discarded and parsing retries.

---

## Parse Tree Output Format

The parse tree is written to the output file in **inorder traversal** order. Each line represents one node with tab-separated fields:

```
Lexeme    LineNo    TokenName    ValueIfNumber    ParentNT    IsLeaf    NodeSymbol
```

| Field | Leaf Node | Internal Node |
|-------|-----------|---------------|
| Lexeme | The actual lexeme string | `----` |
| LineNo | Line number from source | `----` |
| TokenName | Token type name (e.g., `TK_ID`) | `----` |
| ValueIfNumber | Numeric value (for `TK_NUM`/`TK_RNUM`) or `----` | `----` |
| ParentNT | Parent nonterminal name | Parent nonterminal name |
| IsLeaf | `yes` | `no` |
| NodeSymbol | `----` | Nonterminal name |

---

## Test Cases

### Lexer Test Cases (`Lexer Test Cases/Lexer Test Cases/`)

| File | Description |
|------|-------------|
| `t1.txt` | Comprehensive lexer test — all token types, errors, edge cases |
| `t2.txt` | Clean program — no errors, suitable for comment removal testing |
| `lexemesandtokens_t1.txt` | Expected token output for t1.txt |
| `lexemesandtokens_t2.txt` | Expected token output for t2.txt |

### Parser Test Cases (`Parser Test Cases/`)

| File | Description | Expected Result |
|------|-------------|-----------------|
| `t3.txt` | Correct program | "Input source code is syntactically correct." |
| `t4.txt` | Correct program (with `&&&` boolean ops) | "Input source code is syntactically correct." |
| `t5.txt` | Correct program | "Input source code is syntactically correct." |
| `t6.txt` | Program with intentional errors | Multiple syntax errors reported |
| `listoferrors_t6.txt` | Expected errors for t6.txt | Reference for error verification |

### Running Test Cases

```bash
# Lexer test — token list
./stage1exe "Lexer Test Cases/Lexer Test Cases/t1.txt" out.txt
# Select option 2, compare output with lexemesandtokens_t1.txt

# Lexer test — comment removal
./stage1exe "Lexer Test Cases/Lexer Test Cases/t2.txt" out.txt
# Select option 1

# Parser test — correct program
./stage1exe "Parser Test Cases/t3.txt" parsetree_out.txt
# Select option 3

# Parser test — error case
./stage1exe "Parser Test Cases/t6.txt" parsetree_out.txt
# Select option 3, compare errors with listoferrors_t6.txt
```

---

## Implementation Details

| Component | Approach |
|-----------|----------|
| Lexer | Hand-coded DFA with buffer-based file reading |
| Keyword lookup | Linear search through keyword table |
| FIRST sets | Fixed-point iteration with nullable tracking |
| FOLLOW sets | Fixed-point iteration using FIRST sets |
| Parse table | `M[NonTerminal][Terminal] = RuleIndex` (2D array) |
| Parser | Stack-based LL(1) predictive parsing |
| Parse tree | Constructed during parsing, output via inorder traversal |
| Error recovery | Panic-mode with FOLLOW set synchronization |

---

## Authors

CS F363 — Compiler Construction, BITS Pilani