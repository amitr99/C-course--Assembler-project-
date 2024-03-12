#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#include "parser.h"
#include "directives.h"
#include "instructions.h"
#include "labels.h"
#include "utils.h"

/* Creates an empty label table.*/
label_table * createLabelTable() {
    label_table * labelTable = (label_table * ) malloc(sizeof(label_table));
    if (labelTable != NULL) {
        labelTable -> head = NULL;
    }
    return labelTable;
}
/*Checks if a given token matches the expected token type and value*/
boolean match(TokenType expectedType, const char *expectedValue, Token token) {
    return token.type == expectedType && strcmp(token.value.string, expectedValue) == 0;
}


/*only add to table if is label declaration*/
boolean parseLabel(Token token, char ** line, machine_word codeImage[], machine_word dataImage[], label_table *labelTable, boolean isData, boolean isExternal, boolean isEntry, int *IC, int *DC, int lineNumber) {

    Token nextToken;
    int op_count = 0;
    while(token.type != 0)
    {
    char *name = token.value.string;
    if (token.type == LABEL_DECLARATION) 
    {
    	if(token.type == TWO_OPERANDS) 
    	{
        	nextToken = getNextToken(line, lineNumber);
        	op_count++;
        	if(op_count == 2) 
        	{
        		return (parseTwoOperands(line, token, codeImage, IC, DC, lineNumber));
        	}
        } if(token.type == ONE_OPERAND) 
          {
        	nextToken = getNextToken(line, lineNumber);
        	op_count++;
        	if(op_count == 1)
        	{
        		return (parseOneOperand(line, token, codeImage, IC, DC, lineNumber));
        	}
          }
          /*here we get the contents of the label - can get any directive or instruction and then parse according to that*/
        if (token.type == DIRECTIVE) {
            if (parseDirective(token, line, codeImage, dataImage, labelTable, isData, isExternal, isEntry, IC, DC,
                               lineNumber) == FALSE) { /*if the label name is grammatically correct*/
                printError("Invalid input after label name. isDirective - isLabel", lineNumber);
                return FALSE;
            } else {
                if (strcmp(token.value.string, "data") == 0 || strcmp(token.value.string, "string") == 0) {
                    isData = TRUE;
                } else {
                    if (strcmp(token.value.string, ".entry") == 0 || strcmp(token.value.string, ".extern") == 0) {
                        printError("'.entry' or '.extern' cannot appear after label name. - isLabel", lineNumber);
                        return FALSE;
                    }
                }
            }
        }

        if (nextToken.type == COMMA) {
            printError("Comma cannot appear right after label name. - isLabel", lineNumber);
            printf("token type: %d, value: '%s'\n", token.type, token.value.string);
            return FALSE;
        }
        if (nextToken.type == NUMBER || nextToken.type == INVALID) {
            printError("Invalid character after label name. INVALID OR NUMBER - isLabel", lineNumber);
            printf("token type: %d, value: '%s'\n", token.type, token.value.string);
            return FALSE;
        }
        return createLabel(name, labelTable, isExternal, isEntry, isData, IC, DC, lineNumber);
    } else if (isValidLabel(name, token.type, lineNumber)) /*is a label called in the middle of an instruction*/
    	createLabel(token.value.string, labelTable, isExternal, isEntry, isData, IC, DC, lineNumber);
        return TRUE;
    if (nextToken.type != END) {
        /*here we get the contents of the label - can get any directive or instruction and then parse according to that*/
        if (token.type == DIRECTIVE) {
            if ((match(DIRECTIVE, ".data", token)) || match(DIRECTIVE, ".string", token)) {
                isData = TRUE;
                /*if the label name is grammatically correct*/
                if (parseDirective(token, line, codeImage, dataImage, labelTable, isData, isExternal, isEntry, IC, DC,
                                   lineNumber))
                    return createLabel(name, labelTable, isExternal, isEntry, isData, IC, DC, lineNumber);
                else {
                    printf("Error found at line #%d: Invalid input after label name. isDirective - isLabel\n",
                           lineNumber);
                    return FALSE;
                }
            } else {
                if (match(DIRECTIVE, ".entry", token) || match(DIRECTIVE, ".extern", token)) {
                    printf("Error found at line #%d: '.entry' or '.extern' cannot appear after label name. - isLabel\n",
                           lineNumber);
                    return FALSE;
                }
            }
        }/*strcmp(token.value.string, "mov") == 0*/

        if (token.type == TWO_OPERANDS) {
            if (parseTwoOperands(line, token, codeImage, IC, DC, lineNumber))
                return createLabel(name, labelTable, isExternal, isEntry, isData, IC, DC, lineNumber);
            else {
                printf("Error found at line #%d: Invalid input after label name. - isLabel\n", lineNumber);
                return FALSE;
            }
        }
        if (token.type == ONE_OPERAND) {
            if (parseOneOperand(line, token, codeImage, IC, DC, lineNumber))
                return createLabel(name, labelTable, isExternal, isEntry, isData, IC, DC, lineNumber);
            else {
                printf("Error found at line #%d: Invalid input after label name. - isLabel\n", lineNumber);
                return FALSE;
            }
        }
        if (token.type == NO_OPERANDS) {
            if (parseNoOperands(line, token, codeImage, IC, DC, lineNumber))
                return createLabel(name, labelTable, isExternal, isEntry, isData, IC, DC, lineNumber);
            else {
                printf("Error found at line #%d: Invalid input after label name. - isLabel\n", lineNumber);
                return FALSE;
            }
        }
        nextToken = getNextToken(line, lineNumber);
        lineNumber++;
    }

  
  }

	nextToken = getNextToken(line, lineNumber);
    return TRUE;  
}
        
/* Checks if a given string is a valid label name.*/
boolean isValidLabel(char * str, TokenType type, int lineNumber) {
    int i;
    /*check if the label name is valid*/
    if (!isalpha(str[0])) {
        printError("Label should start with a letter. - isValidLabel", lineNumber);
        return FALSE;
    }

    /*check if label is the right length*/
    if (strlen(str) > MAX_LABEL_LENGTH) {
        printError("Label name too long. - isValidLabel", lineNumber);
        return FALSE;
    }

    /*check if the label has only letters and numbers*/
    for (i = 1; i < strlen(str)-1; i++) {
        if (!isalpha(str[i]) && !isdigit(str[i])) {
            printError("Label name should only contain letters or numbers. - isValidLabel", lineNumber);
            return FALSE;
        }
    }

    /*check if label definition ends with colon*/
    if (str[i] != ':' && type==LABEL_DECLARATION){
        printError("Label definition should end with a colon. - isValidLabel", lineNumber);
        return FALSE;
    }

    /*check if label name is a saved name*/
    if (legalLabelName(str) == 2) {
        printError("Illegal label name - cannot be an instruction's name. - isValidLabel", lineNumber);
        return FALSE;
    } else if (legalLabelName(str) == 3) {
        printError("Illegal label name - cannot be a directive's name. - isValidLabel", lineNumber);
        return FALSE;
    }
    return TRUE;
}
/* Searches the label table for a specific label name.*/
boolean findLabel(char * name, label_table *labelTable) {
    label * current;
    boolean isInTable = FALSE;

    current = labelTable->head;
    while (current != NULL) {
        /*Checks if the label exists in the table*/
        if (strcmp(name, current -> name) == 0)
            isInTable = TRUE;
        current = current -> next;
    }
    return isInTable;
}
/* Inserts a new label into the label table.*/
boolean insertLabel(label * newLabel, label_table *labelTable, int lineNumber) {

    /* check for NULL input */
    if (newLabel == NULL) {
        return FALSE;
    }

    /* if the list already has labels, newLabel becomes the new head */
    if (labelTable->head != NULL) {
        newLabel->next = labelTable->head;
    }

    /* update the head of labelTable to point to newLabel */
    labelTable->head = newLabel;
    return TRUE;
}
/* Creates a new label and adds it to the label table.*/
boolean createLabel(char * name, label_table *labelTable, boolean isExternal, boolean isEntry, boolean isData, int *IC, int *DC, int lineNumber) {
    label * newLabel;
    /*Checks if label is already in the table. If yes, sends an error message. If not, adds the new label to the table.*/
    if (findLabel(name + ':', labelTable)) {
        printWarning("Label is already defined. - creatLabel.", lineNumber);
        return FALSE;
    }

    /*Defines a new label and allocates space*/
    newLabel = (label * ) calloc(MAX_LABEL_LENGTH, sizeof(label));

    /*Copies information from file*/
    strcpy(newLabel -> name, name);
    newLabel -> isExternal = isExternal;
    newLabel -> isEntry = isEntry;
    newLabel -> isData = isData;

    /*Updates the IC or DC accordingly*/
    if (!isData)
    
    	newLabel -> address = *IC;
    else
        newLabel -> address = *DC;
        
    

    insertLabel(newLabel, labelTable, lineNumber);

    return TRUE;
}
/* Checks if a label name is a valid label, operation, or instruction name.*/
int legalLabelName(char * name) {
    int currentIn = 0;
    int currentDi = 0;
    int isInstruction = 1;
    int isDirective = 1;

    while (currentIn < NUM_OF_INSTRUCTIONS) {
        /*Checks if the macro name is an operation*/
        if (strcmp(name, instructions[currentIn]) == 0)
            isInstruction = 2;
        currentIn++;
    }

    while (currentDi < NUM_OF_DIRECTIVES) {
        /*Checks if the macro name is a directive*/
        if (strcmp(name, directives[currentDi]) == 0)
            isDirective = 3;
        currentDi++;
    }

    return MAX(isInstruction, isDirective);
}

