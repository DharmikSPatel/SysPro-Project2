#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "lines.h"

// create a function that initially creates a char array using malloc, and for every unique word in the file, add it to the array
// if the word is already in the array, increment the count of that word by keeping track of count in a separate array
// if the word is not in the array, add it to the array and set the count to 1
// at the end, print out the array of words and their counts
char** words;
int* wordCounter;
static int arrayPos = 0;

void countWords(char *fileName) {
    words = malloc(sizeof(char*) * 100);
    wordCounter = malloc(sizeof(int) * 100);
    for (int i = 0; i < 100; i++) {
        words[i] = NULL;
        wordCounter[i] = 0;
    }
    bool wordExists;

    LINES *lines = lopen(fileName);
    char *line;
    while ((line = get_line(lines))) {
        char *word = strtok(line, " ");
        while (word != NULL) {
            wordExists = false;
            printf(">>%s<<\n", word);
            for (int i = 0; i < arrayPos; i++) {
                if (strcmp(words[i], word) == 0) {
                    wordCounter[i]++;
                    wordExists = true;
                    break;
                }
            }
            if (!wordExists) {
                words[arrayPos] = word;
                wordCounter[arrayPos] = 1;
                arrayPos++;
            }
            word = strtok(NULL, " ");
        }

    }
    free(line);
    lclose(lines);
}

int main(int argc, char **argv) {
    char *fileName = argc > 1 ? argv[1] : "input.txt";
    // char *fileName = "input.txt";
    // LINES *lines = lopen(fileName);

    // char *line;
    // while ((line = getline(lines))) {
    //     printf(">>%s<<\n", line);
    //     free(line);
    // }
    // lclose(lines);
    countWords(fileName);
    for (int i = 0; i < arrayPos; i++) {
        printf("%s: %d\n", words[i], wordCounter[i]);
    }
    return EXIT_SUCCESS;
}