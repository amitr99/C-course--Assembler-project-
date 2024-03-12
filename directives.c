#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "directives.h"
#include "labels.h"
#include "utils.h"

/**
 * @brief Handles ".entry" and ".extern" directives, updating the label table.
 * @param token       The directive token.
 * @param line        Pointer to the current line being processed.
 * @param codeImage   Machine words array for instructions.
 * @param dataImage   Machine words array for data.
 * @param labelTable  The table of labels.
 * @param isData      Indicates if the directive is for data.
 * @param isExternal  Indicates if the directive is external.
 * @param isEntry     Indicates if the directive is an entry.
 * @param IC          Instruction counter.
 * @param DC          Data counter.
 * @param lineNumber  Current line number.
 * @return TRUE if successful parsing, FALSE otherwise.
 */
static boolean parseDirectiveExtEnt(Token token, char ** line, machine_word codeImage[], machine_word dataImage[], label_table 
*labelTable, boolean isData, boolean isExternal, boolean isEntry, int *IC, int *DC, int lineNumber);

/**
 * @brief Parses a ".string" directive and generates machine words for string storage.
 * @param token  The token after the ".string" directive.
 * @param line  Pointer to the current line being processed.
 * @param dataImage  Machine words array to store the data.
 * @param IC  Instruction counter.
 * @param DC  Data counter.
 * @param lineNumber  Current line number.
 * @return TRUE if successful parsing, FALSE otherwise.
 */
static boolean parseDirectiveString(Token token, char **line, machine_word dataImage[], const int *IC, int *DC, int lineNumber);
/*************************************************************************************************/

/*Directives */
char * directives[] = {
        ".data",
        ".string",
        ".entry",
        ".extern"
};




/*Processes ".data" directive and generates machine words for data storage.*/
boolean parseDirectiveData(char ** line, machine_word dataImage[], const int *IC, int *DC, int lineNumber) {
    int numberCounter = 0, commaCounter = 0;
    Token token = getNextToken(line, lineNumber);

    /* Check if we have already reached the maximum number of machine words*/
    if ((*IC + *DC) >= MAX_FILE_SIZE) {
        printWarning("Maximum number of machine words (1024) reached.", lineNumber);
        return FALSE;
    }

    while (token.type != END) {
        if (token.type == NUMBER) {
            numberCounter++;
            /*save number*/
            dataImage[*DC].word.data_word.data = token.type & 0xFFF; /*Convert to a 12-bit word */
            dataImage[*DC].word.data_word.line_number = lineNumber;
            dataImage[*DC].wordType = DATA_WORD_TYPE;
            (*DC)++;
        }else if (token.type == COMMA){
            commaCounter++;
        }else {
            printError("Invalid character.", lineNumber);
            return FALSE;
        }

        /*Check if we have reached the maximum number of machine words*/
        if ((*IC + *DC) >= MAX_FILE_SIZE) {
            printWarning("Maximum number of machine words (1024) reached.", lineNumber);
            break;
        }
        token = getNextToken(line, lineNumber);
    }

    if (numberCounter == (commaCounter+1))
        return TRUE;
    else {
        printError("Invalid number of commas.", lineNumber);
        return FALSE;
    }
}



/* Processes a string directive and generates machine words for string storage.*/
static boolean parseDirectiveString(Token token, char **line, machine_word dataImage[], const int *IC, int *DC, int lineNumber) {
    int i = 0, asciiValue;
    int counter = 0;
    /*move to the token after the ".string" directive*/
    token = getNextToken( line, lineNumber);
    /*check if string starts and ends with quotation marks*/
    if (token.value.string[0] != '"' || token.value.string[strlen(token.value.string) - 1] != '"') {
        printError("String should start and end with quotation marks.", lineNumber);
        return FALSE;
    }

    /*check that the next token is the end of the line*/
    token = getNextToken( line, lineNumber);
    if (token.type != END) {
        printError("Invalid character after string.", lineNumber);
        return FALSE;
    }

    /*save the string as data words*/
    while (token.value.string[i] != '\0') {
        asciiValue = (unsigned char)token.value.string[i];
        if ((*IC + counter + *DC) < MAX_MEMORY_SPACE) {
            i++;
            dataImage[*DC + counter].word.data_word.data = asciiValue & 0xFFF; /*Convert to a 12-bit word */
            dataImage[*DC + counter].word.data_word.line_number = lineNumber;
            dataImage[*DC + counter].wordType = DATA_WORD_TYPE;
            counter++;
        } else {
            printWarning("Maximum number of machine words (1024) reached.", lineNumber);
            return FALSE;
        }
    }
    /*adding '\0'*/
    if ((*DC + counter + *IC) < MAX_MEMORY_SPACE) {
        dataImage[*DC + counter].word.data_word.data = '\0'; /*Convert to a 12-bit word */
        dataImage[*DC + counter].word.data_word.line_number = lineNumber;
        counter++;
    } else {
        printWarning("Maximum number of machine words (1024) reached.", lineNumber);
        return FALSE;
    }

    /*update the DC counter*/
    *DC += counter;
    return TRUE;
}


/*Handles ".entry" and ".extern" directives, updating the label table.*/
static boolean parseDirectiveExtEnt(Token token, char ** line, machine_word codeImage[], machine_word dataImage[], label_table 
*labelTable, boolean isData, boolean isExternal, boolean isEntry, int *IC, int *DC, int lineNumber) {
    int tokenCounter = 1;
    boolean isLable = FALSE;

    if (isExternal && isEntry) {
        printError("Label should not be defined both as .entry and as .extern.", lineNumber);
        return FALSE;
    }
    while (token.type != END && tokenCounter <= 3 && token.type != INVALID) {
        if (tokenCounter == 2 && token.type == LABEL) {
        	if(isEntry){
        
            	isLable = parseLabel(token, line, codeImage, dataImage, labelTable, 0, isExternal, TRUE, IC, DC, lineNumber);
            }
            if(isExternal)
            {
            	isLable = parseLabel(token, line, codeImage, dataImage, labelTable, 0, isExternal, TRUE, IC, DC, lineNumber);
            }
        }
        token = getNextToken(line, lineNumber);
        tokenCounter++;
    }
    if(token.type == INVALID) {
        printError("Line has an invalid token.", lineNumber);
        return FALSE;
    }
    if (tokenCounter > 3) {
        printError("After '.entry' or '.extern' only a label name should appear. - directiveExtEnt", lineNumber);
        return FALSE;
    }
    if (!isLable) {
        printError("Label name invalid. - directiveExtEnt", lineNumber);
        return FALSE;
    }
    return TRUE;
}

/*Main function that selects the appropriate parsing function based on the provided directive.*/
boolean parseDirective(Token token, char ** line, machine_word codeImage[], machine_word dataImage[], label_table *labelTable, boolean isData, boolean isExternal, boolean isEntry, int *IC, int *DC, int lineNumber) {
    if (strcmp(token.value.string,  ".data") == 0) {
        return parseDirectiveData(line, dataImage, IC, DC, lineNumber);
    } else if (strcmp(token.value.string,  ".string") == 0) {
        return parseDirectiveString(token, line, dataImage, IC, DC, lineNumber);
    }
    else if (strcmp(token.value.string,  ".entry") == 0) {
        isEntry = TRUE;
        return parseDirectiveExtEnt(token, line, codeImage, dataImage, labelTable, FALSE, isExternal, isEntry, IC, DC, lineNumber);
    } else if (strcmp(token.value.string,  ".extern") == 0) {
        isExternal = TRUE;
        return parseDirectiveExtEnt(token, line, codeImage, dataImage, labelTable, FALSE, isExternal, isEntry, IC, DC, lineNumber);
    } else {
        printError("If a word starts with a dot it must be an directive name.", lineNumber);
        return FALSE;
    }
}
