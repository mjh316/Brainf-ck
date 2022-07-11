#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#define N 30000
#define BUF_SIZE 100000
#define TMPNAM_SIZE 20

int max(int x, int y)
{
    return x > y ? x : y;
}

uint8_t arr[N];
int arrPtr;
int curBracketDepth;
int lines = 1;
char *outputFile = "workingTMPFILE.c";
char *binaryName = "a.out";
char *templateCode =
    "#include <stdio.h>\n"
    "#include <stdint.h>\n"
    "#define N 30000\n"
    "uint8_t arr[N];\n"
    "int arrPtr = 0;\n"
    "int main(void) {\n";

FILE *create_file()
{
    FILE *ptr;
#ifndef DEBUG
    if ((ptr = fopen(outputFile, "r")) != NULL)
    {
        fclose(ptr);
        fprintf(stderr, "WORKFILE ALREADY EXISTS. EXITING\n");
        exit(EXIT_FAILURE);
    }
#else
    remove(outputFile);
#endif
    ptr = fopen(outputFile, "a");
    return ptr;
}

FILE *init_file(FILE *filePtr)
{
    fwrite(templateCode, sizeof(char), strlen(templateCode), filePtr);
    assert(!ferror(filePtr));
    return filePtr;
}

void end_file(FILE *filePtr)
{
    fwrite("}", sizeof(char), 1, filePtr);
}

void write_ptr_increment(FILE *filePtr)
{
    fwrite("arrPtr = (arrPtr+1)%N;\n", sizeof(char), 23, filePtr);
}

void write_ptr_decrement(FILE *filePtr)
{
    fwrite("arrPtr = (!arrPtr?(N-1):arrPtr-1);\n", sizeof(char), 35, filePtr);
}

void write_ptr_value_increment(FILE *filePtr)
{
    fwrite("arr[arrPtr]++;\n", sizeof(char), 15, filePtr);
}

void write_ptr_value_decrement(FILE *filePtr)
{
    fwrite("arr[arrPtr]--;\n", sizeof(char), 15, filePtr);
}

void print_character(FILE *filePtr)
{
    fwrite("putchar(arr[arrPtr]);\n", sizeof(char), 22, filePtr);
}

void scan_character(FILE *filePtr)
{
    fwrite("arr[arrPtr] = getchar();\n", sizeof(char), 25, filePtr);
}

void start_loop(FILE *filePtr)
{
    fwrite("while(arr[arrPtr]) {\n", sizeof(char), 21, filePtr);
}

void end_loop(FILE *filePtr)
{
    end_file(filePtr);
}

void compile()
{
    char *buffer = calloc(70, sizeof(char));
    sprintf(buffer, "gcc %s -Ofast -o %s", outputFile, binaryName);
    FILE *_tmp = popen(buffer, "r");
    assert(_tmp != NULL);
    pclose(_tmp);
#ifndef DEBUG
    remove(outputFile);
#endif
}

void interpret_string(char *buffer)
{
    FILE *workingFile = create_file();
    workingFile = init_file(workingFile);
    int bufPtr = 0;
    int bufLen = strlen(buffer);
    char cur;
    while (bufPtr < bufLen && (cur = buffer[bufPtr]))
    {
        // fprintf(stderr, "Current value of arr[arrPtr]: %d Current instruction: %c\n", arr[arrPtr], cur);
        switch (cur)
        {
        case '>':
        {
            write_ptr_increment(workingFile);
            break;
        }
        case '<':
        {
            write_ptr_decrement(workingFile);
            break;
        }
        case '+':
        {
            write_ptr_value_increment(workingFile);
            break;
        }
        case '-':
        {
            write_ptr_value_decrement(workingFile);
            break;
        }
        case '.':
        {
            print_character(workingFile);
            break;
        }
        case ',':
        {
            scan_character(workingFile);
            break;
        }
        case '[':
        {
            start_loop(workingFile);
            break;
        }
        case ']':
        {
            end_loop(workingFile);
            break;
        }
        }
        ++bufPtr;
    }
    end_file(workingFile);
    assert(!fclose(workingFile));
    compile();
}

void interpret_file(FILE *file)
{
    char *buffer = calloc(BUF_SIZE, sizeof(char));
    fread(buffer, 1, BUF_SIZE - 1, file);
    if (ferror(file))
    {
        printf("Error in reading file, returning.\n");
        return;
    }
    interpret_string(buffer);
}

void interpret_stdin()
{
    char *buffer = calloc(BUF_SIZE, sizeof(char));
    int bufPtr = 0;
    while (bufPtr < BUF_SIZE && (buffer[bufPtr++] = getchar_unlocked()) != EOF)
        ;
    interpret_string(buffer);
}

int main(int argc, char **argv)
{
    FILE *file;
    if (argc > 1 && ((file = fopen(argv[1], "r")) != NULL))
    {
        // if file is passed as argument to program and exists, interpret it
        interpret_file(file);
    }
    else
    {
        // interpret until newline
        interpret_stdin();
    }
    return 0;
}
