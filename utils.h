#ifndef UTILS_H
#define UTILS_H

#define baseAddress 100
#define MAX_MEMORY_SPACE 1024
#define MAX_LINE_LENGTH 80
#define MAX_FILE_NAME_LENGTH 76
#define MAX_FILE_SIZE 100
#define MAX_LABEL_LENGTH 31
#define NUM_OF_DIRECTIVES 4
#define NUM_OF_INSTRUCTIONS 16
#define MAX(A, B)((A > B) ? A : B)
#define BASE_ADD 100

/*Boolean variable*/
typedef enum {
    FALSE = 0,
    TRUE = 1
} boolean;


/*Labels*/
typedef struct Label {
    char name[MAX_LABEL_LENGTH+1]; /*adding one extra space for NULL ending*/
    int address;
    boolean isExternal;
    boolean isEntry; 
    boolean isData;
    struct Label * next;
} label;


/*Label table*/
typedef struct {
    label * head;
} label_table;

/*Lexmemes*/
typedef enum {
    DIRECTIVE,
    ONE_OPERAND,
    TWO_OPERANDS,
    NO_OPERANDS,
    REGISTER,
    NUMBER,
    LABEL_DECLARATION,
    LABEL,
    COMMA,
    DATA,
    END,
    INVALID
} TokenType;

/*Tokens*/
typedef struct {
    TokenType type;
    union value {
        char string[MAX_LABEL_LENGTH + 1];  /*adding one extra space for NULL ending*/
        int integer;
    } value;
} Token;

/*Define the machine word that always comes first*/
typedef struct first_word {
    unsigned int ARE: 2;
    unsigned int dst_op_addr: 3;
    unsigned int src_op_addr: 3;
    unsigned int op_code: 4;
} first_word;

/*Define a machine word with direct or immediate addressing*/
typedef struct immdt_drct_word {
    unsigned int ARE: 2;
    unsigned int operand: 10;
} immdt_drct_word;

/*Define a machine word that is just data*/
typedef struct data_word {
    unsigned int data: 12;
    int line_number;
} data_word;

/*Define a machine word that holds the value and addresses of between one to two registers*/
typedef struct rgstr_word {
    unsigned int ARE: 2;
    unsigned int dst_op_addr: 5;
    unsigned int src_op_addr: 5;
} rgstr_word;

/*Variable to indicate machine word type*/
typedef enum {
    FIRST_WORD_TYPE,
    IMMDT_DRCT_WORD_TYPE,
    DATA_WORD_TYPE,
    RGSTR_WORD_TYPE
} WordType;

/*Define a general machine word - one that is either an instruction, data word, register word or immediate or direct addressing word*/
typedef struct machine_word {
    label label; /*Here we will place the labels in the first pass*/
    boolean isLabel;
    WordType wordType; /*This indicates the type of word */
    union word {
        first_word first_word;
        immdt_drct_word immdt_drct_word;
        data_word data_word;
        rgstr_word register_word;
    } word;
} machine_word;

#endif /* UTILS_H */
