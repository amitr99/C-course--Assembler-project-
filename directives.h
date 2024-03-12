#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#include "parser.h"
#include "instructions.h"
#include "labels.h"
#include "utils.h"


/*Directives */
extern char *directives[4];

/**
 * Processes an directive token and generates machines words appropriately.
 * @param token The directive token.
 * @param line The current line of assembly code.
 * @param codeImage Array to store the machine words for instructions.
 * @param dataImage Array to store the machine words for data.
 * @param labelTable The table of labels.
 * @param isData Indicates if the directive is defined as data.
 * @param isExternal Indicates if the directive is defined as external.
 * @param isEntry Indicates if the directive is defined as entry.
 * @param IC The instruction counter.
 * @param DC The data counter.
 * @param lineNumber The current line number being processed.
 * @return TRUE if processing was successful, FALSE otherwise.
 */
boolean parseDirective(Token token, char ** line, machine_word codeImage[], machine_word dataImage[], label_table *labelTable, boolean isData, boolean isExternal, boolean isEntry, int *IC, int *DC, int lineNumber);


/**
 * @brief Processes ".data" directive and saves the numbers into the data image.
 * @param line Pointer to the current line being processed.
 * @param dataImage Machine words array to store the data.
 * @param IC Instruction counter.
 * @param DC Data counter.
 * @param lineNumber  Current line number.
 * @return TRUE if successful parsing, FALSE otherwise.
 */
boolean parseDirectiveData(char ** line, machine_word dataImage[], const int *IC, int *DC, int lineNumber);

#endif /* DIRECTIVES_H */
