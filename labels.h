#ifndef LABELS_H
#define LABELS_H

#include "parser.h"
#include "directives.h"
#include "instructions.h"
#include "utils.h"

/**
 * Creates an empty label table.
 * @return A pointer to the created label table.
 */
label_table * createLabelTable();

/**
 * Checks if the given token is a valid label and processes its details.
 * @param token The token to be checked.
 * @param line The current line of assembly code.
 * @param codeImage Array to store the machine words for instructions.
 * @param dataImage Array to store the machine words for data.
 * @param labelTable The table of labels.
 * @param isData Indicates if the instruction is defined as data.
 * @param isExternal Indicates if the instruction is defined as external.
 * @param isEntry Indicates if the instruction is defined as entry.
 * @param IC The instruction counter.
 * @param DC The data counter.
 * @param lineNumber The current line number being processed.
 * @return TRUE if processing was successful, FALSE otherwise.
 */
boolean parseLabel(Token token, char ** line, machine_word codeImage[], machine_word dataImage[], label_table *labelTable, boolean isData, boolean isExternal, boolean isEntry, int *IC, int *DC, int lineNumber);

/**
 * Checks if a given string is a valid label name.
 * @param str The string to be checked.
 * @param labelTable The table of labels.
 * @param lineNumber The current line number being processed.
 * @return TRUE if the label name is valid, FALSE otherwise.
 */
boolean isValidLabel(char * str, TokenType type, int lineNumber);

/**
 * Searches the label table for a specific label name.
 * @param name The label name to search for.
 * @param labelTable The table of labels.
 * @return TRUE if the label is found, FALSE otherwise.
 */
boolean findLabel(char * name, label_table *labelTable);

/**
 * Inserts a new label into the label table.
 * @param newLabel The label to be inserted.
 * @param labelTable The table of labels.
 * @return TRUE if insertion was successful, FALSE otherwise.
 */
boolean insertLabel(label * newLabel, label_table *labelTable, int lineNumber);

/**
 * Creates a new label and adds it to the label table.
 * @param name The name of the label.
 * @param labelTable The table of labels.
 * @param isExternal Indicates if the label is defined as external.
 * @param isEntry Indicates if the label is defined as entry.
 * @param isData Indicates if the label is defined as data.
 * @param IC The Instruction Counter.
 * @param DC The Data Counter.
 * @param lineNumber The current line number being processed.
 * @return TRUE if label creation was successful, FALSE otherwise.
 */
boolean createLabel(char * name, label_table *labelTable, boolean isExternal, boolean isEntry, boolean isData, int *IC, int *DC, int lineNumber);

/**
 * Checks if a label name is a valid label, operation, or instruction name.
 * @param name The label name to check.
 * @return 1 if it's a valid label name, 2 if it's an operation name, 3 if it's an instruction name.
 */
int legalLabelName(char * name);

#endif /* LABELS_H */
