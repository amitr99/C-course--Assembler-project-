#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "directives.h"
#include "instructions.h"
#include "labels.h"
#include "utils.h"
#include "writeFiles.h"
#include "preprocessor.h"

 /**
 * Generates an intermediate file name by replacing the extension with ".am".
 * 
 * @param fileName The original file name.
 * @return A dynamically allocated string containing the intermediate file name.
 *         The caller is responsible for freeing the memory.
 */
char* generateIntermediateFileName(const char* fileName) {
    static char intermediateFileName[MAX_FILE_NAME_LENGTH];
    char* fileExtension;
    strcpy(intermediateFileName, fileName);
    fileExtension = strrchr(intermediateFileName, '.'); /* Locate the last dot in the filename */
    if (fileExtension) {
        *fileExtension = '\0'; /* Trim the extension */
    }
    strcat(intermediateFileName, ".am"); /* Append .am to the name */
    return intermediateFileName;
}


int main(int argc, char * argv[]) {
    MacroTable macroTable;
    int i, IC = 0, DC = 0, lineNumber = 1;
    boolean ERROR_FOUND = FALSE;
    machine_word codeImage[MAX_MEMORY_SPACE], dataImage[MAX_MEMORY_SPACE];
    char codeImage64[MAX_MEMORY_SPACE], dataImage64[MAX_MEMORY_SPACE];
    unsigned short codeImageBinary[MAX_MEMORY_SPACE], dataImageBinary[MAX_MEMORY_SPACE];
    char line[MAX_LINE_LENGTH+1]; /*adding one extra space for NULL ending*/
    FILE *outputFile;
    char* intermediateFileName;
    
    label_table labelTable = *createLabelTable();   
    label *head;
    if (argc <= 1) {
        printError("Error - no files in command line.", 0);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        char * fileName = argv[i];
        /*Check if the file name ends with ".as"*/
        if (!(strlen(fileName) > 3 && strcmp(fileName + strlen(fileName) - 3, ".as") == 0)) {
            printf("Skipping file '%s' as it does not have the '.as' extension.\n", fileName);
            continue;
        }
        printf("Processing file: %s\n", fileName);
        lineNumber = 1;
        ERROR_FOUND = FALSE;
        /* MacroTable macroTable;*/
    initializeMacroTable(&macroTable);
    intermediateFileName = generateIntermediateFileName(fileName);
	processSourceFile(fileName, intermediateFileName, &macroTable);
    freeMacroTable(&macroTable);
          
        outputFile = fopen(intermediateFileName, "r"); /* Open the processed output file*/
        if (outputFile == NULL) {
            perror("Error opening the processed output file");
            return 1;
       
        }
       
        /* Process the file end with .am line by line*/
        while (fgets(line, sizeof(line), outputFile) != NULL) {
        
            ERROR_FOUND |= (parseLine(line, codeImage, dataImage, &labelTable, &IC, &DC, lineNumber) == FALSE);
            
            
  
            lineNumber++;
            
         continue;     
             
        }
        /* if no errors were found there creates the files*/
        
       head = labelTable.head;
        while (head!=NULL) {
            printf("%s\n", head->name);
            head=head->next;
        }
      if (!ERROR_FOUND) {
            writeFiles("output.am", codeImage, dataImage, codeImage64, dataImage64, codeImageBinary, dataImageBinary, labelTable, IC, DC);
        }
        /* Close the file*/
        fclose(outputFile);
    }
    return 0;
    }
