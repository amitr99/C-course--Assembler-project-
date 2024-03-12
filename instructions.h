#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "parser.h"
#include "directives.h"
#include "labels.h"
#include "utils.h"

/*Instructions*/
extern char *instructions[16];

/**
 * Processes an instruction with two operands and generates machine words accordingly.
 * @param line The current line of assembly code.
 * @param codeImage Array to store the machine words for instructions.
 * @param IC The instruction counter.
 * @param DC The data counter.
 * @param lineNumber The current line number being processed.
 * @return TRUE if processing was successful, FALSE otherwise.
 */
boolean parseTwoOperands(char **line, Token token, machine_word codeImage[], int *IC, int *DC, int lineNumber);

/**
 * Processes an instruction with one operand and generates machine words accordingly.
 * @param line The current line of assembly code.
 * @param codeImage Array to store the machine words for instructions.
 * @param IC The instruction counter.
 * @param DC The data counter.
 * @param lineNumber The current line number being processed.
 * @return TRUE if processing was successful, FALSE otherwise.
 */
boolean parseOneOperand(char **line, Token token, machine_word codeImage[], int *IC, int *DC, int lineNumber);

/**
 * Processes an instruction with no operands and generates a machine word accordingly.
 * @param line The current line of assembly code.
 * @param codeImage Array to store the machine words for instructions.
 * @param IC The instruction counter.
 * @param DC The data counter.
 * @param lineNumber The current line number being processed.
 * @return TRUE if processing was successful, FALSE otherwise.
 */
boolean parseNoOperands(char ** line, Token token, machine_word codeImage[], int *IC, int *DC, int lineNumber);

#endif /* INSTRUCTIONS_H */
