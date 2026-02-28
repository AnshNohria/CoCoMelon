Use GCC version 13.3.0 and Ubuntu 24.04.2 LTS for compiler project code development. 

Ensure that your code is compatible to GCC and Ubuntu version as specified above. Teams are advised to design data structures for token info, grammar, parse table, parse tree first and follow sets etc. and use names in self explanatory form. Following are the suggested prototypes for better understanding of the implementation needs and are provided as support. However, the teams will have the flexibility to select the prototypes, parameters etc. appropriately.

1. File lexer.c : This file contains following functions

FILE *getStream(FILE *fp): This function takes the input from the file pointed to by 'fp'. This file is the source code written in the given language. The function uses an efficient technique to populate twin buffer by bringing the fixed sized piece of source code into the memory for processing so as to avoid intensive I/O operations mixed with CPU-intensive tasks. The function also maintains the file pointer after every access so that it can get more data into the memory on demand. The implementation can also be combined with getNextToken() implementation as per the convenience of the team. 

tokenInfo getNextToken(twinBuffer B): This function reads the input character stream and uses efficient mechanism to recognize lexemes. The function tokenizes the lexeme appropriately and returns all relevant information it collects in this phase (lexical analysis phase) encapsulated as tokenInfo. The function also displays lexical errors appropriately. 

removeComments(char *testcaseFile, char *cleanFile): This function is an additional plug-in to clean the source code by removal of comments. The function takes as input the source code and writes the clean code in the file appropriately. [Note: This function is invoked only once through your driver file to showcase the comment removal for evaluation purpose. However, your lexer does not really pick inputs from the comment removed file. Rather, it keeps ignoring the comments and keep collecting token Info to pass to the parser. For showcasing your lexers ability, directly take input from user source code]

2. File parser.c : This file contains following functions

FirstAndFollow    ComputeFirstAndFollowSets (grammar G): This function takes as input the grammar G, computes FIRST and FOLLOW information and populates appropriate data structure FirstAndFollow. First and Follow set automation must be attempted, keeping in view the programming confidence of the team members and the available time with the teams.  If teams opt not to develop the module for computation of First and follow sets, the same can be computed manually and information be populated in the data structure appropriately. However, all members of the team must understand that any new grammar rule for any new construct will then require their expertise in computing FIRST and FOLLOW sets manually. 

createParseTable(FirstAndFollow F, table T): This function takes as input the FIRST and FOLLOW  information in F to populate the table T appropriately. 

parseInputSourceCode(char *testcaseFile, table T): This function takes as input the source code file and parses using the rules as per the predictive parse table T and returns a parse tree. The function gets the tokens using lexical analysis interface and establishes the syntactic structure of the input source code using rules in T. The function must report all errors appropriately (with line numbers) if the source code is syntactically incorrect. If the source code is correct then the token and all its relevant information is added to the parse tree. The start symbol of the grammar is the root of the parse tree and the tree grows as the syntax analysis moves in top down way. The function must display a message "Input source code is syntactically correct..........." for successful parsing.

printParseTree(parseTree PT, char *outfile): This function provides an interface for observing the correctness of the creation of parse tree. The function prints the parse tree in inorder in the file outfile. The output is such that each line of the file outfile must contain the information corresponding to the currently visited node of the parse tree in the following format

lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol

The lexeme of the current node is printed when it is the leaf node else a dummy string of characters "‐‐‐‐" is printed. The line number is one of the information collected by the lexical analyzer during single pass of the source code. The token name corresponding to the current node is printed third. If the lexeme is an integer or real number, then its value computed by the lexical analyzer should be printed at the fourth place. Print the grammar symbol (non-terminal symbol) of the parent node of the currently visited node appropriately at fifth place (for the root node print ROOT for parent symbol) . The sixth column is for printing yes or no appropriately. Print the non-terminal symbol of the node being currently visited at the 7th place, if the node is not the leaf node [Print the actual non-terminal symbol and not the enumerated values for the non-terminal]. Ensure appropriate justification so that the columns appear neat and straight.

Description of other files

lexerDef.h : Contains all data definitions used in lexer.c 

lexer.h : Contains function prototype declarations of functions in lexer.c

parserDef.h : Contains all definitions for data types such as grammar, table, parseTree etc. used in parser.c

parser.h : Contains function prototype declarations of functions in parser.c

driver.c : As usual, drives the flow of execution to solve the given problem. (more details, if needed, will be uploaded soon)

makefile : This file uses GNU make utility, which determines automatically which pieces of a large program need to be recompiled, and issues the commands to recompile them. The correctness of your make file depends on file dependencies used correctly.

NOTE:

1. A file using definitions and functions from other files must include interface files appropriately. For example parser.c uses functions of lexer.c, so lexer.h should be included in parser.c. Do not include lexer.h in lexer.c, as lexer.c already has its own function details. Also keep data definitions in files separate from the files containing function prototypes. In case of doubts, meet me and clarify your doubts. It is essential to place the contents in appropriate files and have correct set of files.

2. Use of any high level library other than standard C library is strictly not allowed. 

Your driver must have the following choices. Press option for the defined task (Use a while loop to receive option choices till option 0 is pressed. Ensure independence of working of all options e.g. if option 3 is pressed, option 2 is not needed)

0 : For exit

1 : For removal of comments ‐ print the comment free code on the console

2 : For printing the token list (on the console) generated by the lexer. This option performs lexical analysis and prints all tokens and lexemes line number wise. Here, the tokens are not passed to the parser, but printed on the console only. Each token appears in a new line along with the corresponding lexeme and line number. (invoke only lexer) [Ensure pretty printing with column justifications to increase readability]

3 : For parsing to verify the syntactic correctness of the input source code and printing the parse tree appropriately. This option prints all errors - lexical and syntactic, line number wise, on the console and prints parse tree in the file as mentioned in the command line below. (Invoke both lexer and parser).

4: For printing (on the console) the total time taken by your project code of lexer and parser to verify the syntactic correctness. Use <time.h> file as follows

-------------------------------------------------------------------------------------------------------------

#include <time.h>

clock_t start_time, end_time;

double total_CPU_time, total_CPU_time_in_seconds;

start_time = clock();

// invoke your lexer and parser here

end_time = clock();

total_CPU_time = (double) (end_time - start_time);

total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

// Print both total_CPU_time and total_CPU_time_in_seconds

---------------------------------------------------------------------------------------------------------

Perform actions appropriately by invoking appropriate functions.

Also during its execution, the driver displays necessary information regarding the implementation status of your work at the beginning on the console such as

(a) FIRST and FOLLOW set automated

(b) Only Lexical analyzer module developed

(c) Both lexical and syntax analysis modules implemented

(d) modules compile but give segmentation fault

(e) modules work with testcases 2, 3 and 4 only

(f) parse tree could not be constructed

and so on which ever is applicable.

Compilation:

The name of the make file should be makefile only as I will avoid using ‐f option always to make your file named something else (that includes searching for the file which is time taking). You can find documentation at the GNU website where you can learn how to write a make file (http://www.gnu.org/software/make/manual/make.html).

Please ensure compatibility with the GCC specifications provided in the recent notice.

Execution

The command line argument for execution of the driver should be as follows, for example

$./stage1exe  testcase.txt  parsetreeOutFile.txt

where stage1exe is the executable file generated after linking all the files (your makefile should be absolutely correct). The file testcase.txt is the sourcecode file in the given language to be analyzed and parsetreeOutFile.txt is the file containing parse tree printed as per the format specified earlier.

The inorder traversal for an n‐ary tree can be seen as follows

Leftmost child ‐‐> parent node‐‐> remaining siblings (excluding the leftmost)

A document with detailed analysis of the given grammar has been uploaded for your reference. The groups are advised to verify their grammar and understand the details. The names of additional non-terminals used by the groups (if any) to modify grammar rules need not to be changed, provided they convey the meaning of the constructs and preserve the features of the language.

Please inform me if any discrepancy is observed in this modified document.

Regards,

Instructor, CS F363


    t3.txt: This test case is syntactically correct. Ensure that your  parser works efficiently and produces the message for successful compilations as mentioned earlier. A parser creates the parse tree while in the parsing process. When the source code is syntactically correct in the given test case, the parser successfully builds the parse tree. Also, if in any test case there are syntactic errors, the parser recovers them and continues building the parse tree despite the fact that it lost a corresponding subtree while going for panic mode error recovery.
    t4.txt: This test case is syntactically correct. 
    t5.txt : This test case is syntactically correct. In case of any error, please inform me so that the error can be eliminated.
    t6.txt: This test case has errors. All errors are required to be reported line number wise. The order of printing the errors may differ due to the heuristic used by the teams for panic mode error recovery. Also, the panic mode of recovery might skip some errors from being reported. Ensure that you report the maximum errors. Include the semicolon and a few keywords like endrecord, endunion, endif, endwhile, else, ), ] etc. in your synchronization set. This will give you better error reporting.  However, it is expected that you maintain the initial format  given in file listoferrors_t6.txt.  You can follow the same style of the error message or you may have your own message content that is self explanatory. Do not repeatedly report the same error multiple times at the same line number due to error recovery or any other issue. 

The teams are advised to create their own testcases handling all features given in the language specification document and as per the modified grammar.  I advise you to gradually go for larger and complex test cases starting with very simple 5 to 10 lines of code with very minimal set of features . Keep testing your modules with features added incrementally to the test case that you use while creating your lexer and parser. 

Please inform me if any discrepancy is observed in the testcases uploaded here or in the previous test cases for lexical analyzer.

1. Project code submission is due on March 02, 2026 (Monday)) during 05:30 p.m. to 08:30 p.m. Ensure that you are not submitting any piece of plagiarized code.

2. A submission with wrong group number will not be considered for evaluation.

3. Any one member of the team will be allowed to submit the code.

4. All members of the team are advised to be present together (in-person or online as per their convenience) at the time of code submission. This is to ensure that the submission includes all files correctly and does not have plagiarized code in it.

5. Create a folder and name it as your group number (Group_01, Group_12, ....etc.). This folder must contain the following files

    grammar.txt
    coding details.pdf (Download the attached docx file, type in the details and submit the pdf)
    lexerDef.h
    lexer.c
    lexer.h
    parserDef.h
    parser.h
    parser.c
    driver.c
    makefile
    testcases (should be available in your group folder directly)
    any other files(token file, auxiliary functions, file containing first and follow sets if you are manually computing these, and so on which you might have used)

6. Group folder should not have any subfolder in it.

7. Do not include *.o and stage1exe in the above folder for uploading.

8. Files for each of the testcases generated by you should be named using the prefix testcase followed by the number, followed by the extension 'txt', e.g. testcase5.txt, testcase2.txt, etc. 

9. Submit only those testcases with which your code works well. I will be using my testcases as well for evaluation, but asking you to submit the testcases gives me a scope to verify your lexer and parser with your testcases as well.

10. Ensure that the makefile is named as makefile (and no other name is used for it). Check the file dependencies to ensure correct working of the makefile.

11. Ensure that the driver supports the five options for execution as were earlier mentioned.

12. Verify that each file (except any input file you use in your code) has in its first 5-6 lines your group number, Ids and names of  all team members in comment. Do not forget to comment these. Unnamed files will not be evaluated.

13. Ensure the compatibility of your code with the GCC specifications given earlier.

14. Refresh the course webpage (Nalanda) to verify that you have not missed out any updates. Go through all details and ensure that nothing is left out which you would have preferred to incorporate in your code.

15. Compress the group folder using zip ( of type group_#.zip, where # represents your group number ) to upload. (Other formats such as tar, gz, and rar etc. will  not be accepted)

16. Fill up the given proforma with the necessary details.

17. Ensure that the code you are intending to submit is not plagiarized.

18. Any one of the team members' Nalanda account should be used for submission. A duplicate submission may carry a penalty in terms of deduction of marks.

19. Submit your file Group_#.zip (# represents your group number) through the course page (https://nalanda.bits-pilani.ac.in/). The link will be made available as per the specified date and time. Avoid last minute submission. 

20. Submission will be complete after successfully uploading the zip file as per the above details.  

21. Teams not following the submission guidelines, will be penalized appropriately.

The following test cases of lexical analyzer and a few output samples are given for your reference.

    t1.txt: This test case has lexemes following the given patterns and should be used to test your lexical analyzer. More patterns and unknown symbols can be added to verify the working of your lexer. The file lexemesandtokens_t1.txt displays the format and expected output of your lexer on the test case t1.txt. 
    t2.txt: This test case is syntactically correct. How the output appears on the console on comment removal is shown in the file commentremoval_t2.txt. Note that the appearance of the source code does not change after the removal of the comments. The indentations, white spaces etc. in the non-commented lines and the line numbers of the code remain same after comment removal.  Also, the lexemes and tokens are displayed with line numbers in file lexemesandtokens_t2.txt.

Another suggestion is to create larger test cases of about 400 to 600 lines, which you can use to test the efficiency of your compiler project code in terms of time and memory needs. If you re using twin buffer, try to have a small block size of 50 bytes for each buffer and test the refilling of the two buffers alternatively. A compiler developer should not take the whole memory at the disposal of the compiler program, and in case of very large user programs (for us 400-600 lines, in actual million lines of code to be compiled), the buffer helps. 

Please inform me if any discrepancy is observed in the testcases uploaded here.

