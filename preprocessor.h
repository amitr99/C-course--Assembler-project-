#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LENGTH_OF_MCRO 4
#define MAX_LINE_LEN 81

/**
 * @struct Macro
 * @brief Represents a single macro with its name and content.
 *
 * @var name     The name of the macro. Assumes a maximum name length of MAX_LINE_LENGTH.
 * @var content  Pointer to the content of the macro.
 */
typedef struct Macro {
    char name[MAX_LINE_LEN];
    char *content;
} Macro;

/**
 * @struct MacroTable
 * @brief Table structure to store and manage multiple macros.
 *
 * @var macros   Pointer to the array of Macro structures.
 * @var count    Current number of macros in the table.
 * @var capacity Total capacity of the macro table.
 */
typedef struct MacroTable {
    Macro *macros;
    int count;
    int capacity;
} MacroTable;

/**
 * @brief Initializes a macro table with default values.
 *
 * @param table Pointer to the MacroTable to be initialized.
 */
void initializeMacroTable(MacroTable *);

/**
 * @brief Finds a macro by its name in the given macro table.
 *
 * @param table Pointer to the MacroTable.
 * @param name  Name of the macro to be found.
 * @return Returns the index of the found macro, or -1 if not found.
 */
int findMacro(const MacroTable *, const char *);

/**
 * @brief Adds a new macro to the macro table.
 *
 * @param table   Pointer to the MacroTable.
 * @param name    Name of the new macro.
 * @param content Content of the new macro.
 */
void addMacro(MacroTable *,  char *,  char *);

/**
 * @brief Frees the memory allocated for the macro table and its content.
 *
 * @param table Pointer to the MacroTable to be freed.
 */
void freeMacroTable(MacroTable *);

/**
 * @brief Processes a source file for macro replacements using the given macro table.
 *
 * @param inputFilename  Name of the source file to be processed.
 * @param outputFilename Name of the output file where the processed content will be written.
 * @param table          Pointer to the MacroTable containing the macros.
 */
void processSourceFile( char *,  char *,  MacroTable *);

/**
 * @brief Checks if the given string is a valid macro name.
 *
 * @param name Name of the macro to be validated.
 * @return Returns 1 if valid, 0 otherwise.
 */
int isValidMacroName(const char *);

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * @param str Pointer to the string to be trimmed.
 */
void trimWhitespace(char *);
