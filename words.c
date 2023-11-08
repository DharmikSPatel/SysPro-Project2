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

char** words;
int* wordCounter;
static int arrayPos = 0;

void initializeArrays() {
    words = malloc(sizeof(char*) * 100);
    wordCounter = malloc(sizeof(int) * 100);
    for (int i = 0; i < 100; i++) {
        words[i] = NULL;
        wordCounter[i] = 0;
    }
}

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

void countWordsInFile(char *fileName) {
    bool wordExists;
    LINES *lines = lopen(fileName);
    char *line;

    while ((line = get_line(lines))) {
        char *word = strtok(line, " \n");
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
            word = strtok(NULL, " \n");
        }

    }
    free(line);
    lclose(lines);
}

// void countWordsInFile(char* filename);

/**
 * @brief This function recursivly opens and reads from a directory
 * 
 * @param fullPath Full path of the folder
 * @param level The amount of tabs to append
 */
void dir(const char* fullPath, int level){
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
                dir(fname, level+1);
                break;
            case TYPE_FILE:
                printf("FILE: %s\n", fname);
                countWordsInFile(fname);
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
int main(int argc, char const *argv[])
{
    if(argc <= 0) {
        printf("please re run with args\n"); 
        return EXIT_FAILURE;
    }
    initializeArrays();
    for(int i = 1; i < argc; i++){
        const char* fname = argv[i];
        int type = isFileOrDir(fname);
        switch (type){
            case TYPE_DIR:
                printf("DIR: %s\n", fname);
                dir(fname, 0);
                break;
            case TYPE_FILE:
                printf("FILE: %s\n", fname);
                countWordsInFile(fname);
                break;
            default:
                printf("OTHER: %s\n", fname);
                break;
        }
    }
    for(int i = 0; i < arrayPos; i++){
        printf("%s: %d\n", words[i], wordCounter[i]);
    }
    return EXIT_SUCCESS;
}

