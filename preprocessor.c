#include "preprocessor.h"

void processSourceFile(char *sourceFileName, char *outputFileName, MacroTable *macroTable) {
	
	char line[MAX_LINE_LEN];
	int isInsideMacro = 0;
	char macroName[MAX_LINE_LEN];
    char *macroContent = NULL;
    size_t macroContentSize=0;
    int index;
    char trimmedLine[MAX_LINE_LEN];
    int macroIndex;
    char *commentStart;
    
    FILE *sourceFile = fopen(sourceFileName, "r");
    FILE *outputFile = fopen(outputFileName, "w");
    
    if(sourceFile == NULL || outputFile == NULL) {
    	printf("Error opening files.\n");
    	return;
    }
    
    while (sourceFile != NULL && fgets(line, sizeof(line), sourceFile)) {
    	/*Skip empty or comment line*/
    	commentStart = strchr(line, ';');
    	if (commentStart) {
        *commentStart = '\0';  /*Truncate the string*/
    	}
    	if(strspn(line, "\t\n\r\f\v") == strlen(line)) {
    		continue;
    	}
    	
    	/*Check for macro start*/
    	if(strstr(line, "mcro") && !strstr(line, "endmcro")) {
    		sscanf(line + LENGTH_OF_MCRO, "%s", macroName);
    		if(isValidMacroName(macroName) && findMacro(macroTable, macroName) == -1) {
                isInsideMacro = 1;
                continue;
             }
         }
         
         /*Check for macro end*/
         if (strstr(line, "endmcro") && isInsideMacro) {
            isInsideMacro = 0;
            index = findMacro(macroTable, macroName);
            if (index == -1) {
                addMacro(macroTable, macroName, macroContent);
            } else {
                printf("Macro '%s' is already defined.\n", macroName);
            }
            
            free(macroContent); /*Free the macro content memory*/
            macroContent = NULL;
            macroContentSize = 0;
            continue;
        }
        
        /*Reading inside macro*/
        if (isInsideMacro) {
            size_t lineLength = strlen(line);
            macroContent = realloc(macroContent, macroContentSize + lineLength + 1);
            if (!macroContent) {
                printf("Failed to allocate memory for macro content.\n");
                exit(1);
            }
            strcpy(macroContent + macroContentSize, line);
            macroContentSize += lineLength;
            continue;
        }
        
        /*Check if the line is a call to a macro*/
        sscanf(line, "%s", trimmedLine); /*This will remove leading and trailing spaces*/
        macroIndex = findMacro(macroTable, trimmedLine);
        if (macroIndex != -1) {
            if (macroIndex >= 0 && macroIndex < macroTable->count) {
                fputs(macroTable->macros[macroIndex].content, outputFile);
            } else {
                printf("Invalid macro index: %d\n", macroIndex);
            }
            continue;
        }
        
        /*Write the line to the output file*/
        fputs(line, outputFile);
    }

    fclose(sourceFile);
    fclose(outputFile);
}


 /* Initialize a macro table with initial capacity */
void initializeMacroTable(MacroTable *table) {
    table->count = 0;
    table->capacity = 10;
    table->macros = (Macro *)malloc(table->capacity * sizeof(Macro));  /* Allocate memory for the macros array*/
    if (table->macros == NULL) {
        printf("Failed to allocate memory for macro table.\n");
        free(table);
        exit(1);
    }
}

/* Trim leading and trailing whitespace characters from a string */
void trimWhitespace(char *str) {
    int length = strlen(str);
    int start = 0;
    int end = length - 1;
    int newLength =0;

    /* Find the first non-whitespace character from the beginning */
    while (start < length && isspace(str[start])) {
        start++;
    }

    /* Find the last non-whitespace character from the end */
    while (end >= 0 && isspace(str[end])) {
        end--;
    }

    /* Calculate the new length after trimming */
     newLength = end - start + 1;
    if (newLength < 0) {
        newLength = 0;
    }

    /* Move the trimmed content to the beginning of the string */
    memmove(str, str + start, newLength);

    /* Add a null terminator at the end to terminate the trimmed string */
    str[newLength] = '\0';
}


/* This method return -1 if is not a macro, else reeturn the index of the macro in the macro table */
int findMacro(const MacroTable *table, const char *name) {
    char trimmedName[MAX_LINE_LEN];
    int i;
    strcpy(trimmedName, name);
    trimWhitespace(trimmedName);

    
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->macros[i].name, trimmedName) == 0) {
            
            return i; /* Macro found */
        }
    }
    
    return -1; /* Macro not found */
}
/* this method ensure that the name of the macro is not as same as name of an instruction or prompt*/
int isValidMacroName(const char *name) {
    int i;

    /* List of reserved names */
    const char *reserved[] = {
        ".", "lea", "sub", "add", "cmp", "mov", "not", "clr", "inc", "dec",
        "jmp", "bne", "red", "prn", "jsr", "stop", "rts"
    };

    /* Calculate the number of reserved words */
    int numReserved = sizeof(reserved) / sizeof(reserved[0]);

    /* Check if name starts with a dot */
    if (name[0] == '.') {
        return 0;  /* Not valid */
    }

    /* Check if name is in the list of reserved words */
    for (i = 0; i < numReserved; i++) {
        if (strcmp(name, reserved[i]) == 0) {
            return 0;  /* Not valid */
        }
    }

    return 1;  /* Valid */
}

/* This method is add the macro to the macro table if the macro is valid*/
void addMacro(MacroTable *table,  char *name,  char *content) {
    int index = findMacro(table, name);
    Macro *newMacros = realloc(table->macros, table->capacity * sizeof(Macro));
    if (newMacros == NULL) {
    	printf("Failed to reallocate memory for macro table.\n");
        free(newMacros);
        exit(1);
    }
    
    table->macros = newMacros;
    if (index == -1) {
        if (table->count == table->capacity) {
            /* Double the capacity of the macro table*/
            table->capacity *= 2;
        }
        strcpy(table->macros[table->count].name, name);
        /*strcpy(table->macros[table->count].content, content);*/
        
   

     table->macros[table->count].content = malloc(strlen(content) + 1);
        if (table->macros[table->count].content == NULL) {
            printf("Failed to allocate memory for macro content.\n");
            exit(1);
        }
        strcpy(table->macros[table->count].content, content);
        table->count++;
    } else {
        printf("Macro '%s' is already defined.\n", name);
    }
}

void freeMacroTable(MacroTable *table) {
    free(table->macros);
}


