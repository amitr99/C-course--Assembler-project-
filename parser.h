#ifndef PARSER_H
#define PARSER_H

#include "directives.h"
#include "instructions.h"
#include "labels.h"
#include "utils.h"

/**
 * Parses a line of assembly code and processes its tokens.
 * @param line The line of assembly code to be parsed.
 * @param codeImage An array to store the machine word for instructions.
 * @param dataImage An array to store the machine word for data.
 * @param labelTable The table of labels.
 * @param IC The instruction counter.
 * @param DC The data counter.
 * @param lineNumber The current line number being processed.
 * @return TRUE if parsing was successful, FALSE otherwise.
 */
boolean parseLine(char *line, machine_word codeImage[], machine_word dataImage[], label_table *labelTable, int *IC, int *DC, int lineNumber);

/**
 * Tokenizes the input line to extract the next token.
 * @param line A pointer to the input line.
 * @param lineNumber The current line number.
 * @return The extracted token.
 */
Token getNextToken(char ** line, int lineNumber);


/**
* Skips whitespace characters in a string and returns a pointer to the next non-whitespace character.
 * @param ch Pointer to the string where spaces are to be skipped.
 * @returns char* Pointer to the next non-whitespace character in the string. If the string only contains spaces or is empty, the returned pointer might point to a NULL character.
 */
char *skipSpaces(char *ch);

/**
 * Prints error messages with line numbers for better traceability.
 * @param error Error message to be printed.
 * @param lineNumber The current line number where the error occurred.
 */
void printError (char* error, int lineNumber);

/**
 * Prints warning messages with line numbers for better awareness.
 * @param warning Warning message to be printed.
 * @param lineNumber The current line number where the warning occurred.
 */
void printWarning (char* warning, int lineNumber);

#endif /*PARSER_H*/
