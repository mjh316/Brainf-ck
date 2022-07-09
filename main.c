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


int max(int x, int y) {
    return x > y ? x : y;
}

uint8_t arr[N];
int arrPtr;
int curBracketDepth;
int lines = 1;

void interpret_string(char *buffer) {
    int bufPtr = 0;
    int bufLen = strlen(buffer);
    char cur;
    while (bufPtr < bufLen && (cur = buffer[bufPtr])) {
        // fprintf(stderr, "Current value of arr[arrPtr]: %d Current instruction: %c\n", arr[arrPtr], cur);
        switch (cur) {
            case '>': {
                if (arrPtr + 1 == N) {
                    // fprintf(stderr, "Reached max array index, pointer increment failed. Overflowing\n");
                    arrPtr = 0;
                } else {
                    ++arrPtr;
                }
                break;
            }
            case '<': {
                if (arrPtr == 0) {
                    // fprintf(stderr, "Reached minimum array index, pointer decrement failed. Underflowing\n");
                    int i = N - 1;
                    arrPtr = i;
                } else {
                    --arrPtr;
                }
                break;
            }
            case '+': {
                ++arr[arrPtr];
                break;
            }
            case '-': {
                --arr[arrPtr];
                break;
            }
            case '.': {
                putchar_unlocked(arr[arrPtr]);
                break;
            }
            case ',': {
                arr[arrPtr] = getchar_unlocked();
            }
            case '[': {
                // fprintf(stderr, "open bracket index: %d of depth %d\n", bufPtr, curBracketDepth);
                ++curBracketDepth;           
                if (arr[arrPtr] == 0) {
                    int tmp = curBracketDepth;
                    ++bufPtr;
                    while (bufPtr < bufLen && !(buffer[bufPtr] == ']' && tmp == curBracketDepth)) {
                        if (buffer[bufPtr] == '[') ++tmp;
                        if (buffer[bufPtr] == ']' && tmp != curBracketDepth) --tmp;
                        ++bufPtr;
                    }
                    // fprintf(stderr, "buffer[bufPtr]: %d at index: %d\n", buffer[bufPtr], bufPtr);
                    assert(bufPtr < bufLen);
                    assert (buffer[bufPtr] == ']');
                    // fprintf(stderr, "skipping at open bracket at index %d\n",  bufPtr);
                }
                break;
            }
            case ']': {
                // fprintf(stderr, "curBracketDepth: %d on line: %d\n", curBracketDepth, lines);
                // fprintf(stderr, "close bracket index: %d of depth %d\n", bufPtr, curBracketDepth);
                assert(curBracketDepth > 0);
                if (arr[arrPtr] != 0) {
                    int tmp = curBracketDepth;
                    --bufPtr;
                    while (bufPtr >= 0 && !(buffer[bufPtr] == '[' && tmp == curBracketDepth)) {
                        if (buffer[bufPtr] == ']') {
                            // fprintf(stderr, "Found close bracket at index %d. cur tmp %d\n", bufPtr, tmp);
                            ++tmp;
                        }
                        if (buffer[bufPtr] == '[' && tmp != curBracketDepth) {
                            // fprintf(stderr, "Found non matching open bracket at index %d. cur tmp %d\n", bufPtr, tmp);
                            --tmp;
                        }
                        --bufPtr;
                    }
                    // fprintf(stderr, "tmp: %d\n", tmp);
                    assert(bufPtr >= 0);
                    assert(buffer[bufPtr] = '[');
                    //fprintf(stderr, "Going back to depth %d\n", curBracketDepth);
                } else {
                    --curBracketDepth;
                }
                break;
            }
            case '\n': {
                lines++;
                break;
            }
        }
        ++bufPtr;
    }
}

void interpret_file(FILE *file) {
    char *buffer = calloc(BUF_SIZE, sizeof(char));
    fread(buffer, 1, BUF_SIZE - 1, file);
    if (ferror(file)) {
        printf("Error in reading file, returning.\n");
        return;
    }
    interpret_string(buffer);
}

void interpret_stdin() {
    char *buffer = calloc(BUF_SIZE, sizeof(char));
    int bufPtr = 0;
    while ((buffer[bufPtr++] = getchar_unlocked()) != EOF);
    interpret_string(buffer);
}

int main(int argc, char **argv) {
    FILE *file;
    if (argc > 1 && ((file = fopen(argv[1], "r")) != NULL)) {
        // if file is passed as argument to program and exists, interpret it
        interpret_file(file);
    } else {
        // interpret until newline
        interpret_stdin();
    }
    return 0;
}



