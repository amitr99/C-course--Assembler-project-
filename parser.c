#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parser.h"
#include "directives.h"
#include "instructions.h"
#include "labels.h"
#include "utils.h"

/**
* Validates if a given token represents a number and checks if it doesn't exceed a specified bit limit (e.g., 12 bits).
 * @param token The token structure being examined.
 * @param lineNumber The current line number being processed.
 * @returns TRUE If the token represents a valid number within constraints.
 * @returns FALSE If the token is not a valid number or exceeds constraints.
 */
static boolean isNumber(Token token, int lineNumber);


/*************************************************************************************************/


/* Checks if a line of assembly code exceeds the maximum allowed length.*/
static boolean isLineTooLong(const char * line, int lineNumber) {
    int charCount = 0;
    boolean ERROR_FLAG = FALSE;
    while (line[charCount] != '\0') {
        charCount++;
        /*check if line length exceeds 80 characters*/
        if (charCount > MAX_LINE_LENGTH) {
            printError("Line is longer than 80 characters.", lineNumber);
            ERROR_FLAG = TRUE;
        }
    }
    return ERROR_FLAG;
}


/*Validates if a given token is a number and doesn't exceed 12 bits.*/
static boolean isNumber(Token token, int lineNumber) {
    int i = 0;
    int num = 0;
    int sign = 1;


    if (token.value.string[0] == '-' || token.value.string[0] == '+') {
        if (token.value.string[0] == '-') {
            sign = -1;
        }
        i++;
    }

    while (isdigit(token.value.string[i])) {
        num = num * 10 + (token.value.string[i] - '0');
        i++;

        if (num > 2047) {
            printError("Number exceeds 12 bits.", lineNumber);
            return FALSE;
        }
    }

    if (token.value.string[i] == '\0') {
        num *= sign;
        return TRUE;
    } else {
        printError("Invalid number.", lineNumber);
        return FALSE;
    }
}

/* Skips spaces and returns the next non-space character in a string.*/
char *skipSpaces(char *ch)
{
    if(ch == NULL) /*checks if empty*/
        return NULL;
    while(isspace(*ch))
        ch++;
    return ch;
}

/*Extracts the next token from a line of assembly code.*/
Token getNextToken(char **line, int lineNumber) {
    Token token;
    int length;
    char *colonIndex = NULL;
    char *quotationIndex = NULL;
    
    *line = skipSpaces(*line);

    /*check if the line ended*/
    if (**line == '\n' || **line == '\0') {
        token.type = END;
        strcpy(token.value.string, "end of line");
        return token;
    }
    if(**line == ','){
        token.type = COMMA;
        strcpy(token.value.string, "comma");
        (*line)++;
        return token;
    }

    /* Copy the token value */
    length = 0;
    while (!isspace(**line) && **line != '\0' && **line != ',') {
        token.value.string[length] = **line;
        length++;
        (*line)++;
    }
    token.value.string[length] = '\0';

    if((token.value.string[0] == '+' || token.value.string[0] == '-' || isdigit(token.value.string[0])) && isNumber(token, lineNumber)) {
        token.type = NUMBER;
        token.value.integer = atoi(token.value.string);
        return token;
    }
    
    quotationIndex = (strchr(token.value.string, '"') && strrchr(token.value.string, '"'));
    if(quotationIndex)
    {
    	token.type = DATA;
    	return token;	
    }

    /* Check for labels */
    colonIndex = strchr(token.value.string, ':');
    if (colonIndex != NULL) {
        /* If there is a colon somewhere in the line, there has to be a label definition there */
        token.type = LABEL_DECLARATION;
        if (isValidLabel(token.value.string, LABEL_DECLARATION, lineNumber) == FALSE) {
            printError("A colon must appear right after the label definition.", lineNumber);
            token.type = INVALID;
        }
        *colonIndex = '\0';;
        return token;
    }

    if (length >= MAX_LABEL_LENGTH) {
        printError("Line too long.", lineNumber);
        token.type = INVALID;
        return token;
    }
    


    /* Determine the token type based on the token value */
    if (strcmp(token.value.string, "mov") == 0 || strcmp(token.value.string, "cmp") == 0 ||
        strcmp(token.value.string, "add") == 0 || strcmp(token.value.string, "sub") == 0 ||
        strcmp(token.value.string, "lea") == 0) {
        token.type = TWO_OPERANDS;
    } else if (strcmp(token.value.string, "not") == 0 || strcmp(token.value.string, "clr") == 0 || strcmp(token.value.string, "inc") == 0 ||
               strcmp(token.value.string, "dec") == 0 || strcmp(token.value.string, "jmp") == 0 || strcmp(token.value.string, "bne") == 0 || strcmp(token.value.string, "red") == 0 ||
               strcmp(token.value.string, "prn") == 0 || strcmp(token.value.string, "jsr") == 0) {
        token.type = ONE_OPERAND;
    } else if (strcmp(token.value.string, "rts") == 0 || strcmp(token.value.string, "stop") == 0) {
        token.type = NO_OPERANDS;
    } else if (token.value.string[0] == '.') {
        token.type = DIRECTIVE;
    } else if (isalpha(token.value.string[0])) {
        token.type = LABEL;
    } else if (token.value.string[0] == '@') {
        token.type = REGISTER;
    } else {
        token.type = INVALID;
    }

    if(token.type == INVALID)  exit(1);
    return token;
}

/*Parses a given line of assembly code and populates code and data images.*/
boolean parseLine(char * line, machine_word codeImage[], machine_word dataImage[], label_table *labelTable, int* IC, int* DC, int lineNumber) {
    boolean NO_ERROR_FLAG = TRUE;
    Token token;
    char *line_index = line;

    if(isLineTooLong(line, lineNumber)) {
        printError("Could not process file because line exceeded the maximum length limit.", lineNumber);
        return FALSE;
    }

    /*if line is within legal limit then parses it*/
    token = getNextToken(&line_index, lineNumber);

    /*check the first token - the rest of the tokens in the line will be checked in the appropriate functions*/
    if(token.type == LABEL_DECLARATION) {
        NO_ERROR_FLAG = parseLabel(token, &line_index, codeImage, dataImage, labelTable, FALSE, FALSE, FALSE, IC, DC, lineNumber);
        /*isData, isEntry and isExtern are all FALSE at this point because the label is at the beginning of the line*/
        token = getNextToken(&line_index, lineNumber);
    }
    if(NO_ERROR_FLAG == FALSE) {
        return NO_ERROR_FLAG;
    }
    switch (token.type) {
    	case END:
    		break;
        case DIRECTIVE:
            NO_ERROR_FLAG = parseDirective(token, &line_index, codeImage, dataImage, labelTable, FALSE, FALSE, FALSE, IC, DC, lineNumber);
            break;
        case ONE_OPERAND:
            NO_ERROR_FLAG = parseOneOperand(&line_index, token, codeImage, IC, DC, lineNumber);
            break;
        case TWO_OPERANDS:
            NO_ERROR_FLAG = parseTwoOperands(&line_index, token, codeImage, IC, DC, lineNumber);
            break;
        case NO_OPERANDS:
            NO_ERROR_FLAG = parseNoOperands(&line_index, token, codeImage, IC, DC, lineNumber);
            break;
        default:
            printError("Line cannot start with the character given.", lineNumber);
            return FALSE;
    }
  
    return NO_ERROR_FLAG;
}

/*Prints error messages with line number.*/
void printError (char* error, int lineNumber) {
    printf("\033[1;31mERROR\033[0m - \033[1;34mline #%d\033[0m:  %s.\n", lineNumber, error);
}

/*Prints warning messages with line numbers*/
void printWarning (char* warning, int lineNumber) {
    printf("\033[1;33mWARNING\033[0m - \033[1;32mline #%d\033[0m: %s.\n", lineNumber, warning);
}
