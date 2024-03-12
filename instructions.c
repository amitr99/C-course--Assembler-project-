#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "instructions.h"
#include "utils.h"

/*Instructions*/
char * instructions[] = {
        "mov",
        "cmp",
        "add",
        "sub",
        "lea",
        "not",
        "clr",
        "inc",
        "dec",
        "jmp",
        "bne",
        "red",
        "prn",
        "jsr",
        "rts",
        "stop"
};

static boolean isRegister(Token * token, int lineNumber) {
    /*check if the number of the register is within range*/
    if ( token->value.string[1] == 'r' &&  token->value.string[2] >= '0' && token->value.string[2] <= '7' && token->value.string[3] == '\0')
    {
        return TRUE;
    }
    else
        printError("Invalid register.", lineNumber);
    return FALSE;
}


static int findOpcode(Token token) {
    if (strcmp(token.value.string, "mov") == 0)
        return 0;
    if (strcmp(token.value.string, "cmp") == 0)
        return 1;
    if (strcmp(token.value.string, "add") == 0)
        return 2;
    if (strcmp(token.value.string, "sub") == 0)
        return 3;
    if (strcmp(token.value.string, "not") == 0)
        return 4;
    if (strcmp(token.value.string, "clr") == 0)
        return 5;
    if (strcmp(token.value.string, "lea") == 0)
        return 6;
    if (strcmp(token.value.string, "inc") == 0)
        return 7;
    if (strcmp(token.value.string, "dec") == 0)
        return 8;
    if (strcmp(token.value.string, "jmp") == 0)
        return 9;
    if (strcmp(token.value.string, "bne") == 0)
        return 10;
    if (strcmp(token.value.string, "red") == 0)
        return 11;
    if (strcmp(token.value.string, "prn") == 0)
        return 12;
    if (strcmp(token.value.string, "jsr") == 0)
        return 13;
    if (strcmp(token.value.string, "rts") == 0)
        return 14;
    if (strcmp(token.value.string, "stop") == 0)
        return 15;
    return -1;
}

boolean parseTwoOperands(char ** line, Token token, machine_word codeImage[], int *IC, int *DC, int lineNumber) {
    int operandCount = 0;
    int commaCount = 0;
    int ARE = 0; /*absolute addressing*/
    int i;
    int opCode = findOpcode(token);
    if (opCode == -1) {
        printError("Invalid instruction.", lineNumber);
        return FALSE;
    }
    /*write first word*/
    codeImage[*IC].wordType = FIRST_WORD_TYPE;
    codeImage[*IC].word.first_word.ARE = ARE;
    codeImage[*IC].word.first_word.op_code = opCode;

    /*Check if we have reached the maximum number of machine words*/
    while ( ** line != '\0' && *(IC + *DC) < MAX_MEMORY_SPACE) {
        token = getNextToken( line, lineNumber);
        if (token.type == END)
            break;
        operandCount++;
        if (token.type == REGISTER) {
            if (isRegister( &token, lineNumber)) {
                /*check if is a valid register*/
                if (operandCount == 1) {
                    if (opCode == 0 || opCode == 1 || opCode == 2 || opCode == 3) {
                        codeImage[*IC - operandCount].word.first_word.src_op_addr = 5; /*update the first word*/
                        codeImage[*IC].wordType = IMMDT_DRCT_WORD_TYPE; /*update the next word*/
                        codeImage[*IC].word.register_word.src_op_addr = 0;
                        codeImage[*IC].word.register_word.dst_op_addr = atoi(token.value.string + 2); /*Skip the '@' character*/
                    } else {
                        printError("Instruction does not match the operand type entered.", lineNumber);
                        return FALSE;
                    }
                } else if (operandCount == 2) {
                    if (opCode == 0 || opCode == 1 || opCode == 2 || opCode == 3 || opCode == 6) {
                        codeImage[*IC - -operandCount].wordType = FIRST_WORD_TYPE;  /*update the first word*/
                        codeImage[*IC - operandCount].word.first_word.dst_op_addr = 5;
                        codeImage[*IC].wordType = RGSTR_WORD_TYPE; /*update the next word*/
                        codeImage[*IC].word.register_word.dst_op_addr = atoi(token.value.string + 2); /*skip the '@' character*/
                    } else {
                        printError("Instruction does not match the operand type entered.", lineNumber);
                        return FALSE;
                    }
                } else /*if operandCount is invalid*/ {
                    printError("Invalid number of operands.", lineNumber);
                    return FALSE;
                }
            } else /*don't need to print error because it was already printed*/
                return FALSE;
        }

        if (token.type == NUMBER) {
            if (operandCount == 1 && (opCode == 0 || opCode == 1 || opCode == 2 || opCode == 3)) {
                codeImage[*IC - operandCount].word.first_word.dst_op_addr = 1; /*update the first word*/
                codeImage[*IC].wordType = IMMDT_DRCT_WORD_TYPE; /*update the next word*/
                codeImage[*IC].word.immdt_drct_word.operand = atoi(token.value.string);
            }
            if ( operandCount == 2 && opCode == 1 && commaCount == 1) {
                codeImage[*IC - -operandCount].wordType = FIRST_WORD_TYPE; /*update the first word*/
                codeImage[*IC - operandCount].word.first_word.dst_op_addr = 1;
                codeImage[*IC].wordType = IMMDT_DRCT_WORD_TYPE; /*update the next word*/
                codeImage[*IC].word.immdt_drct_word.operand = atoi(token.value.string);
            }
        }

        if (token.type == LABEL) {
            if (operandCount == 1) {
                if (opCode == 0 || opCode == 1 || opCode == 2 || opCode == 3 || opCode == 6) {
                    codeImage[*IC - operandCount].wordType = FIRST_WORD_TYPE; /*update the first word*/
                    codeImage[*IC - operandCount].word.first_word.dst_op_addr = 3;
                    codeImage[*IC].wordType = IMMDT_DRCT_WORD_TYPE; /*update the next word*/
                    codeImage[*IC].isLabel = TRUE; /*update the isLabel flag to signal that there is a label*/
                    for (i=0; i<strlen(token.value.string); i++) {/*place label name in word*/
                        codeImage[*IC].label.name[i] = token.value.string[i];
                    }
                } else {
                    printError("Instruction does not match the operand type entered.", lineNumber);
                    return FALSE;
                }
            } else if (operandCount == 2) {
                if (opCode == 0 || opCode == 1 || opCode == 2 || opCode == 3 || opCode == 6) {
                    codeImage[*IC - operandCount].wordType = FIRST_WORD_TYPE; /*update the first word*/
                    codeImage[*IC - operandCount].word.first_word.dst_op_addr = 3;
                    codeImage[*IC].wordType = IMMDT_DRCT_WORD_TYPE; /*update the next word*/
                    codeImage[*IC].isLabel = TRUE; /*update the isLabel flag to signal that there is a label*/
                    for (i=0; i<strlen(token.value.string); i++) /*place label name in word*/
                        codeImage[*IC].label.name[i] = token.value.string[i];
                } else {
                    printError("Instruction does not match the operand type entered.", lineNumber);
                    return FALSE;
                }
            }
            else /*if operandCount is invalid*/ {
                return FALSE;
            }

        }

        if (token.type == COMMA) {
            commaCount++;
            operandCount--;
        }
    }
    (*IC) += operandCount;

    /*check for errors and warnings*/
    if ((*IC + *DC) >= MAX_MEMORY_SPACE) {
        printWarning("Maximum number of machine words (1024) reached.", lineNumber);
        return FALSE;

    }

    /*check if there were the correct number of commas*/
    if (commaCount > 1) {
        printError("Too many commas.", lineNumber);
        return FALSE;
    }

    /* Check if the correct number of operands is provided*/
    if (operandCount != 2) {
        printError("Wrong number of operands for the instruction inputted.", lineNumber);
        return FALSE;
    }
    return TRUE;
}

boolean parseOneOperand(char ** line, Token token, machine_word codeImage[], int *IC, int *DC, int lineNumber) {
    int operandCount = 0;
    int commaCount = 0;
    int ARE = 0; /*absolute addressing*/
    int i;
    int opCode = findOpcode(token);

    if (opCode == -1) {
        printError("Invalid instruction.", lineNumber);
        return FALSE;
    }
    /*write first word*/
    codeImage[*IC].wordType = FIRST_WORD_TYPE;
    codeImage[*IC].word.first_word.ARE = ARE;
    codeImage[*IC].word.first_word.op_code = opCode;
    codeImage[*IC].word.first_word.src_op_addr = 0;

    /*Check if we have reached the maximum number of machine words*/
    while ( ** line != '\0' && (*IC + *DC) < MAX_MEMORY_SPACE) {
        token = getNextToken( line, lineNumber);
        if (token.type == END)
            break;
        operandCount++;
        if (token.type == REGISTER) {
            if (isRegister( & token, lineNumber)) {
                /*check if is a valid register*/
                if (operandCount == 1) {
                    if (opCode == 4 || opCode == 5 || opCode == 7 || opCode == 8 || opCode == 9 || opCode == 10 || opCode == 11 || opCode == 12 || opCode == 13) {
                        codeImage[*IC - operandCount].word.first_word.src_op_addr = 5; /*update the first word*/
                        codeImage[*IC].wordType = IMMDT_DRCT_WORD_TYPE; /*update the next word*/
                        codeImage[*IC].word.register_word.src_op_addr = 0;
                        codeImage[*IC].word.register_word.dst_op_addr = atoi(token.value.string + 2); /* Skip the '@' character*/
                    } else {
                        printError("Instruction does not match the operand type entered.", lineNumber);
                        return FALSE;
                    }
                } else /*if operandCount is invalid*/ {
                    printError("Invalid number of operands.", lineNumber);
                    return FALSE;
                }
            } else /*don't need to print error because it was already printed*/
                return FALSE;
        }

        if (token.type == NUMBER) {
            if (operandCount == 1 && opCode == 12) {
                codeImage[*IC - operandCount].word.first_word.dst_op_addr = 1; /*update the first word*/
                codeImage[*IC].wordType = IMMDT_DRCT_WORD_TYPE; /*update the next word*/
                codeImage[*IC].word.immdt_drct_word.operand = atoi(token.value.string);
            } else {
                printError("Invalid input.", lineNumber);
                return FALSE;
            }
        }

        if (token.type == LABEL) {
            if (operandCount == 1) {
                if (opCode == 4 || opCode == 5 || opCode == 7 || opCode == 8 || opCode == 9 || opCode == 10 || opCode == 11 || opCode == 12 || opCode == 13) {
                    codeImage[*IC - operandCount].wordType = FIRST_WORD_TYPE; /*update the first word*/
                    codeImage[*IC - operandCount].word.first_word.dst_op_addr = 3;
                    codeImage[*IC].wordType = IMMDT_DRCT_WORD_TYPE; /*update the next word*/
                    codeImage[*IC].isLabel = TRUE; /*update the isLabel flag to signal that there is a label*/
                    for (i=0; i<strlen(token.value.string); i++) {/*place label name in word*/
                        codeImage[*IC].label.name[i] = token.value.string[i];
                    }
                } else {
                    printError("Instruction does not match the operand type entered.", lineNumber);
                    return FALSE;
                }
            } else /*if operandCount is invalid*/
                return FALSE;
        }

        if (token.type == COMMA) {
            commaCount++;
            operandCount--;
        }
    }
    (*IC) += operandCount;

    /*check for errors and warnings*/
    if ((*IC + *DC) >= MAX_MEMORY_SPACE) {
        printWarning("Maximum number of machine words (1024) reached.", lineNumber);
        return FALSE;

    }

    /*check if there were the correct number of commas*/
    if (commaCount > 0) {
        printError("Too many commas.", lineNumber);
        return FALSE;
    }

    /* Check if the correct number of operands is provided*/
    if (operandCount > 1) {
        printError("Too many operands for the instruction inputted.", lineNumber);
        return FALSE;
    }
    return TRUE;
}

boolean parseNoOperands(char ** line, Token token, machine_word codeImage[], int *IC, int *DC, int lineNumber) {
    int ARE = 0; /*absolute addressing*/
    int opCode = findOpcode(token);

    if (opCode == -1) {
        printError("Invalid instruction.", lineNumber);
        return FALSE;
    }

    /*write first word*/
    codeImage[*IC].wordType = FIRST_WORD_TYPE;
    codeImage[*IC].word.first_word.ARE = ARE;
    codeImage[*IC].word.first_word.op_code = opCode;
    codeImage[*IC].word.first_word.src_op_addr = 0;

    /*Check if we have reached the maximum number of machine words*/
    while ( **line != '\0' && (*IC+*DC) < MAX_MEMORY_SPACE) {
        token = getNextToken( line, lineNumber);
        if (token.type != END) {
            printError("Too many operands for the instruction inputted.", lineNumber);
            return FALSE;
        }
    }
    (*IC)++;
    /*check for errors and warnings*/
    if ((*IC+*DC) >= MAX_MEMORY_SPACE) {
        printWarning("Maximum number of machine words (1024) reached.", lineNumber);
        return FALSE;
    }
    return TRUE;
}
