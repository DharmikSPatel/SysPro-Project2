#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include "lines.h"


#define TYPE_FILE 0
#define TYPE_DIR 1
#define TYPE_OTHER -1

struct Word {
    char* word;
    int count;
    struct Word* nextWord;
};

/**
 * @brief This utility function determins if a path is a file, or dir, or neither.
 * 
 * @param fullFilePath Full path of the directory entry 
 * @return int IS_FILE | IS_DIR | IS_OTHER
 */
int isFileOrDir(const char* fullFilePath){
    struct stat stats;
    if(stat(fullFilePath, &stats) == 0){
        if (S_ISREG(stats.st_mode)) 
            return TYPE_FILE;
        else if (S_ISDIR(stats.st_mode))
            return TYPE_DIR;
    }
    return TYPE_OTHER;
}
/**
 * @brief Prints \t a giving amount of times. Used to visulaize directory structure
 * 
 * @param levels the number of \t to append
 */
void printTabs(int levels){
    for(int i = 0; i < levels; i++)
        printf("\t");
}

/**
 * @brief This utility function checks if a directory entry is . or .. or hiden(starts with .)
 * Serves as a base case for the recursion
 * 
 * @param name The name of the file
 * @return int 1 if it is, 0 if it is not
 */
int isItSelfOrParrentDirOrHidden(char* name){

    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0){
        return 1;
    }
    if(name[0] == '.'){
        return 1;
    }
    return 0;
}
int fileIsTextFile(char* fileName){
    int len = strlen(fileName);
    if(len >= 4){
        //printf("%s\n", &(fileName[len - 4]));
        if(strcmp(&(fileName[len - 4]), ".txt") == 0){
            return 1;
        }
    }
    return 0;
}
/**
 * @brief The file must be check by the caller to be a .txt file
 * 
 * @param fileName the name of a .txt file
 * @param head 
 */
void countWordsInFile(char* fileName, struct Word **head) {
    bool wordExists;
    LINES *lines = lopen(fileName);
    char *line;

    while ((line = get_line(lines))) {
        char *word = strtok(line, " \n");
        while (word != NULL) {
            wordExists = false;
            struct Word *currentWord = *head;
            while (currentWord != NULL) {
                if (currentWord->word != NULL && word != NULL && strcmp(currentWord->word, word) == 0) {
                    currentWord->count++;
                    wordExists = true;
                    break;
                }
                currentWord = currentWord->nextWord;
            }
            if (!wordExists) {
                struct Word *newWord = malloc(sizeof(struct Word));
                newWord->word = word;
                newWord->count = 1;
                newWord->nextWord = NULL;
                if (*head == NULL) {
                    *head = newWord;
                }
                else {
                    struct Word *lastWord = *head;
                    while (lastWord->nextWord != NULL) {
                        lastWord = lastWord->nextWord;
                    }
                    lastWord->nextWord = newWord;
                }
            }
            word = strtok(NULL, " \n");
        }

    }
    free(line);
    lclose(lines);
}

void sortList(struct Word *head) {
    // sort the list by count first, then alphabetically
    struct Word *i, *j;
    for (i = head; i != NULL && i->nextWord != NULL; i = i->nextWord) {
        for (j = i->nextWord; j != NULL; j = j->nextWord) {
            if (i->count < j->count || (i->count == j->count && strcmp(i->word, j->word) > 0)) {
                char *tempWord = i->word;
                int tempCount = i->count;
                i->word = j->word;
                i->count = j->count;
                j->word = tempWord;
                j->count = tempCount;
            }
        }
    }
}

void printList(struct Word *head) {
    sortList(head);
    struct Word *currentWord = head;
    while (currentWord != NULL) {
        printf("%s: %d\n", currentWord->word, currentWord->count);
        currentWord = currentWord->nextWord;
    }
}

void freeList(struct Word *head) {
    struct Word *currentWord = head;
    struct Word *nextWord;
    while (currentWord != NULL) {
        nextWord = currentWord->nextWord;
        free(currentWord);
        currentWord = nextWord;
    }
}

// void countWordsInFile(char* filename);

/**
 * @brief This function recursivly opens and reads from a directory
 * 
 * @param fullPath Full path of the folder
 * @param level The amount of tabs to append
 */
void dir(const char* fullPath, int level, struct Word **head){
    printf("Reading %s\n", fullPath);
    DIR *dirp = opendir(fullPath);
    int dlen = strlen(fullPath);
    struct dirent *de;
    while((de = readdir(dirp))){

        if(isItSelfOrParrentDirOrHidden(de->d_name))
            continue;

        int flen = strlen(de->d_name);
        char* fname = malloc(dlen + 1 + flen + 1);
        memcpy(fname, fullPath, dlen);
        fname[dlen] = '/';
        memcpy(fname + dlen + 1, de->d_name, flen + 1);
        int type = isFileOrDir(fname);
        printTabs(level);

        switch (type){
            case TYPE_DIR:
                printf("DIR: %s\n", fname);
                dir(fname, level+1, head);
                break;
            case TYPE_FILE:
                printf("FILE: %s\n", fname);
                if(fileIsTextFile(fname))
                    countWordsInFile(fname, head);
                break;
            default:
                printf("OTHER: %s\n", fname);
                break;
        }
        free(fname);
    }

    closedir(dirp);
}

/**
 * @brief This the runner class. 
 * It will take in a single file name or a single folder name, or a set of files and folder names.
 * 
 * If its a folder, then it will search through the folder recursivly.
 * If its a file, it will make sure its a .txt and then update the counts.
 * 
 * @return int 
 */
int main(int argc, char *argv[])
{
    if(argc <= 0) {
        printf("please re run with args\n"); 
        return EXIT_FAILURE;
    }
    // initializeArrays();
    struct Word *head = malloc(sizeof(struct Word));
    head = NULL;
    for(int i = 1; i < argc; i++){
        char* fname = argv[i];
        int type = isFileOrDir(fname);
        switch (type){
            case TYPE_DIR:
                printf("DIR: %s\n", fname);
                dir(fname, 0, &head);
                break;
            case TYPE_FILE:
                printf("FILE: %s\n", fname);
                if(fileIsTextFile(fname))
                    countWordsInFile(fname, &head);
                break;
            default:
                printf("OTHER: %s\n", fname);
                break;
        }
    }
    printList(head);
    freeList(head);
    return EXIT_SUCCESS;
}

