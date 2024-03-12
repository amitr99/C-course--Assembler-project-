#ifndef WRITEFILES_H
#define WRITEFILES_H


/**
 * Writes label information to entry and extern files.
 * 
 * @param entryFile Pointer to the entry file.
 * @param externFile Pointer to the extern file.
 * @param labelTable The label table containing label information.
 */
 
void writeLabelFiles(FILE* entryFile, FILE* externFile, label_table labelTable);

/**
 * Writes output files with provided data.
 * @param objFileName The object file name.
 * @param extFileName The extern file name.
 * @param entFileName The entry file name.
 * @param codeImage Array of machine words for code.
 * @param dataImage Array of machine words for data.
 * @param codeImage64 Array of base64-encoded code.
 * @param dataImage64 Array of base64-encoded data.
 * @param codeImageBinary Array of binary representation for code.
 * @param dataImageBinary Array of binary representation for data.
 * @param labelTable The table of labels.
 * @param IC The instruction counter.
 * @param DC The data counter.
 */
void writeFiles(const char * fileName, machine_word codeImage[], machine_word dataImage[], char codeImage64[], char dataImage64[], unsigned short codeImageBinary[], unsigned short dataImageBinary[], label_table labelTable, int IC, int DC);

#endif /*WRITEFILES_H*/
