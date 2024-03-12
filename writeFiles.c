#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "directives.h"
#include "instructions.h"
#include "labels.h"
#include "utils.h"
#include "writeFiles.h"

/**
 * Generates output file names based on the input file name.
 * @param inputFileName The input file name.
 * @param entFileName Output for the entry file name.
 * @param extFileName Output for the extern file name.
 * @param objFileName Output for the object file name.
 */

static void generateOutputFileNames(const char * inputFileName, char * entFileName, char * extFileName, char * objFileName);
/**
 * Converts a machine_word to a 12-bit binary representation.
 * @param machineWord The machine word to convert.
 * @param externFile External file to write data.
 * @param entryFile Entry file to write data.
 * @param labelTable The table of labels.
 * @param lineNumber The current line number.
 * @return The 12-bit binary representation.
 */
static unsigned short convertToBinary(machine_word * machineWord, FILE * externFile, FILE * entryFile, label_table labelTable, int lineNumber);
/**
 * Converts a binary word to a base64 representation.
 * @param binaryWord The binary word to convert.
 * @return The base64 representation.
 */
static char * binaryToBase64(unsigned short binaryWord);

void printBinaryRepresentation(unsigned short number);
/**
 * Converts machine_word arrays to binary and base64 representation.
 * @param codeImage Array of machine words for code.
 * @param dataImage Array of machine words for data.
 * @param codeImageBinary Array of binary representation for code.
 * @param dataImageBinary Array of binary representation for data.
 * @param codeImage64 Array of base64-encoded code.
 * @param dataImage64 Array of base64-encoded data.
 * @param externFile External file to write data.
 * @param entryFile Entry file to write data.
 * @param labelTable The table of labels.
 * @param IC The instruction counter.
 * @param DC The data counter.
 */
static void convertArrays(machine_word codeImage[], machine_word dataImage[], unsigned short codeImageBinary[], unsigned short dataImageBinary[], char codeImage64[], char dataImage64[], FILE * externFile, FILE * entryFile, label_table labelTable, int IC, int DC);

/*************************************************************************************************/

/* Generates output file names based on input file name */
static void generateOutputFileNames(const char * inputFileName, char * entFileName, char * extFileName, char * objFileName) {

    char nameWithoutExtension[MAX_FILE_NAME_LENGTH];
    char *lastDot = strrchr(inputFileName, '.'); /* Find the last dot in the filename */

    /* Ensure the filename ends with ".am" */
    if (lastDot && strcmp(lastDot, ".am") == 0) {
        int lengthWithoutExtension = lastDot - inputFileName; /* Calculate the length till the dot */

        /* Make sure the calculated length is within the boundary */
        if (lengthWithoutExtension < MAX_FILE_NAME_LENGTH - 4) { /* Account for null-termination and safety margin */
            strncpy(nameWithoutExtension, inputFileName, lengthWithoutExtension);
            nameWithoutExtension[lengthWithoutExtension] = '\0'; /* Null-terminate the string */
        } else {
            /* Handle the error: filename too long */
            fprintf(stderr, "Filename too long.\n");
            return;
        }
    } else {
        /* Handle the error: incorrect or unexpected file extension */
        fprintf(stderr, "Unexpected file extension. Expected \".am\".\n");
        return;
    }

    /* Create the file names for the output files */
    snprintf(entFileName, MAX_FILE_NAME_LENGTH, "%s.ent", nameWithoutExtension);
    snprintf(extFileName, MAX_FILE_NAME_LENGTH, "%s.ext", nameWithoutExtension);
    snprintf(objFileName, MAX_FILE_NAME_LENGTH, "%s.obj", nameWithoutExtension);
}



/* Converts a machine word to its binary representation */
static unsigned short convertToBinary(machine_word * machineWord, FILE * externFile, FILE * entryFile, label_table labelTable, int lineNumber) {
    unsigned short binary = 0;

    if (machineWord -> isLabel) {
        /* Check if the label is in the table */
        if (findLabel(machineWord -> label.name, &labelTable)) {
            /* Print address to the appropriate file (extern or entry) */
            if (machineWord -> label.isExternal)
                fprintf(externFile, "%s\t%d", machineWord -> label.name, machineWord -> label.address);
            if (machineWord -> label.isEntry || !(machineWord -> label.isExternal))
                fprintf(entryFile, "%s\t%d", machineWord -> label.name, machineWord -> label.address);
        } else {
            printf("Error found at line #%d: Invalid label name.\n", lineNumber);
            return binary;
        }
    } else {
        /* Handle different word types */
        switch (machineWord -> wordType) {
            case FIRST_WORD_TYPE:
                binary |= ((machineWord -> word.first_word.ARE & 0x3) << 10) |
                          ((machineWord -> word.first_word.dst_op_addr & 0x7) << 7) |
                          ((machineWord -> word.first_word.op_code & 0xF) << 3) |
                          (machineWord -> word.first_word.src_op_addr & 0x7);
                break;
            case IMMDT_DRCT_WORD_TYPE:
                binary |= ((machineWord -> word.immdt_drct_word.ARE & 0x3) << 10) |
                          (machineWord -> word.immdt_drct_word.operand & 0x3FF);
                break;
            case DATA_WORD_TYPE:
                binary |= machineWord -> word.data_word.data & 0xFFF; /*Ensured 12 bits*/
                break;
            case RGSTR_WORD_TYPE:
                binary |= ((machineWord -> word.register_word.ARE & 0x3) << 10) |
                          ((machineWord -> word.register_word.dst_op_addr & 0xF) << 6) | /*Adjusted for 12 bits*/
                          (machineWord -> word.register_word.src_op_addr & 0xF); /*Adjusted for 12 bits*/
                break;
            default:
                printf("Error found at line #%d: Unknown word type.\n", lineNumber);
                break;
        }
    }

    return binary & 0xFFF; /*Ensuring the result is 12 bits*/
}

/* Converts a binary word to its base64 representation */
static char * binaryToBase64(unsigned short binaryWord) {
    static const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char * base64Word = (char * ) malloc(3 * sizeof(char)); /*2 chars + null-terminator*/

    /*Ensure that binaryWord is only 12 bits*/
    binaryWord &= 0xFFF;

    /*Extract each 6-bit group and map it to a base64 character*/
    base64Word[0] = base64Chars[(binaryWord >> 6) & 0x3F];  /*get the leftmost 6 bits*/
    base64Word[1] = base64Chars[binaryWord & 0x3F];         /*get the rightmost 6 bits*/
    base64Word[2] = '\0';                                  /*null-terminator*/

    return base64Word;
}

/* Prints the binary representation of a number */
void printBinaryRepresentation(unsigned short number) {
    int i;
    for (i = 11; i >= 0; i--) {
        putchar((number & (1 << i)) ? '1' : '0');
    }
    putchar('\n');
}

/* Converts arrays of machine words to their binary and base64 representations */
static void convertArrays(machine_word codeImage[], machine_word dataImage[], unsigned short codeImageBinary[], unsigned short dataImageBinary[], char codeImage64[], char dataImage64[], FILE * externFile, FILE * entryFile, label_table labelTable, int IC, int DC) {
    int i;

    /* converting the codeImage */
    for (i = 0; i < IC; i++) {
        codeImageBinary[i] = convertToBinary(&codeImage[i], externFile, entryFile, labelTable, i);
        printf("Binary representation of word %d: ", i + 1);
        printBinaryRepresentation(codeImageBinary[i]);
    }
    
    /* converting the dataImage */
    for (i = 0; i < DC; i++) {
        dataImageBinary[i] = convertToBinary(&dataImage[i], externFile, entryFile, labelTable, i);
        printf("Binary representation of word %d: ", i + 1);
        printBinaryRepresentation(dataImageBinary[i]);
    }

    for (i = 0; i < IC; i++) {
        char *base64Rep = binaryToBase64(codeImageBinary[i]);
        strcpy(&codeImage64[i * 2], base64Rep);  /*Assuming codeImage64 is large enough*/
        printf("Base64 representation: %s\n", base64Rep);
        free(base64Rep);  /*Free allocated memory*/
    }


    for (i = 0; i < DC; i++) {
        char *base64Rep = binaryToBase64(dataImageBinary[i]);
        strcpy(&dataImage64[i * 2], base64Rep);  /*Assuming dataImage64 is large enough*/
        printf("Base64 representation: %s\n", base64Rep);
        free(base64Rep);  /*Free allocated memory*/
    }

    codeImage64[IC * 2] = '\0';  /*Null-terminate the strings*/
    dataImage64[DC * 2] = '\0';
}

/* Writing label files based on the label table, given the updated label and label_table structures */
void writeLabelFiles(FILE* entryFile, FILE* externFile, label_table labelTable) {
    label *current = labelTable.head;
    while (current) {
        if (current->isEntry) {
            fprintf(entryFile, "%s %d\n", current->name, current->address);
        } else if (current->isExternal) {
            fprintf(externFile, "%s %d\n", current->name, current->address);
        }
        current = current->next;
    }
}

/* Writes machine code and data to output files */
void writeFiles(const char* fileName, machine_word codeImage[], machine_word dataImage[], 
                char codeImage64[], char dataImage64[], unsigned short codeImageBinary[], 
                unsigned short dataImageBinary[], label_table labelTable, int IC, int DC) {

    int i;
    char entFileName[MAX_FILE_NAME_LENGTH];
    char extFileName[MAX_FILE_NAME_LENGTH];
    char objFileName[MAX_FILE_NAME_LENGTH];
    
    FILE *objFile, *externFile, *entryFile;

    generateOutputFileNames(fileName, entFileName, extFileName, objFileName);

    objFile = fopen(objFileName, "w");
    if (objFile == NULL) {
        perror("Error opening the .obj file.\n");
        return;
    }

    externFile = fopen(extFileName, "w");
    if (externFile == NULL) {
        perror("Error opening the .ext file.\n");
        return;
    }

    entryFile = fopen(entFileName, "w");
    if (entryFile == NULL) {
        perror("Error opening the .ent file.\n");
        return;
    }

    fprintf(objFile, "%d %d\n", IC, DC);

    convertArrays(codeImage, dataImage, codeImageBinary, dataImageBinary, codeImage64, 
                  dataImage64, externFile, entryFile, labelTable, IC, DC);

    for (i = 0; i < IC; i++) {
        fprintf(objFile, "%d:\t %s\n", i , &codeImage64[i]);
    }

    for (i = 0; i < DC; i++) {
        fprintf(objFile, "%d:\t %s\n", IC + i, &dataImage64[i]);
    }

    writeLabelFiles(entryFile, externFile, labelTable);

    fclose(objFile);
    fclose(externFile);
    fclose(entryFile);
}




