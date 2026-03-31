#include <dialog.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_FOLDER_NAME 255

typedef struct fEntry {
    /* The File data */

    // The file's name
    char* fname;
    // If true, the file is a directory
    bool isDir;
    
    // The file's permissions
    bool sticky, setUID, setGID, ownR, ownW, ownX,
    groupR, groupW, groupX, otherR, otherW, otherX;

    /* The Linked List Data */

    // The next item in the list
    struct fEntry* next;
    // The length of the list from the current item to the last
    int len;
} fEntry;

// Provides an linked list of all files and directories in pwd
fEntry* listDir(char* dir) {
    fEntry* dirContent = NULL;
    DIR* workingDir = opendir(dir);

    if (workingDir == NULL) {
        printf("Could not open directory %s with code %d", dir, errno);
        exit(-1);
    }

    struct dirent* file;
    // Read each entry an create an fEntry for each
    while ((file = readdir(workingDir))) {
        struct stat buff;
        int status;

        status = stat(file->d_name, &buff);

        if (status == -1) {
            fprintf(stderr, "Error: failed to get status of file %s with code %d\n", file->d_name, errno);
            exit(-1);
        }
        
        // If the list is empty, allocate and record the file metadata
        if (dirContent == NULL) {
            dirContent = malloc(sizeof(fEntry));

            // Check for malloc errors
            if (dirContent == NULL) {
                fprintf(stderr, "Error: Failed to allocate memory for head of linked list with code %d\n", errno);
                exit(-1);
            }
            // Record linked list data
            dirContent->len = 1;
            dirContent->next = NULL;
            // Record file metadata
            dirContent->fname = file->d_name;
            dirContent->isDir = (buff.st_mode & S_IFDIR);
            // Record file perms
            dirContent->sticky = (buff.st_mode & S_ISVTX) != 0;
            dirContent->setGID = (buff.st_mode & S_ISGID) != 0;
            dirContent->setUID = (buff.st_mode & S_ISUID) != 0;
            dirContent->ownR = (buff.st_mode & S_IRUSR) != 0;
            dirContent->ownW = (buff.st_mode & S_IWUSR) != 0;
            dirContent->ownX = (buff.st_mode & S_IXUSR) != 0;
            dirContent->groupR = (buff.st_mode & S_IRGRP) != 0;
            dirContent->groupW = (buff.st_mode & S_IWGRP) != 0;
            dirContent->groupX = (buff.st_mode & S_IXGRP) != 0;
            dirContent->otherR = (buff.st_mode & S_IROTH) != 0;
            dirContent->otherW = (buff.st_mode & S_IWOTH) != 0;
            dirContent->otherX = (buff.st_mode & S_IXOTH) != 0;
        } 
        // If not, walk to the current end of the list, allocate, then record
        else {

            fEntry *lNode = dirContent;
            while (lNode->next != NULL) {
                // Increment the length
                lNode->len++;
                lNode = lNode->next;
            }

            lNode->next = malloc(sizeof(fEntry));

            // Check for malloc errors
            if (lNode->next == NULL) {
                fprintf(stderr, "Error: Failed to allocate memory for node of linked list with code %d\n", errno);
                exit(-1);
            }
            // Record linked list data
            lNode->next->len = 1;
            lNode->next->next = NULL;
            // Record file metadata
            lNode->next->fname = file->d_name;
            lNode->next->isDir = (buff.st_mode & S_IFDIR);
            // Record file perms
            lNode->next->sticky = (buff.st_mode & S_ISVTX) != 0;
            lNode->next->setGID = (buff.st_mode & S_ISGID) != 0;
            lNode->next->setUID = (buff.st_mode & S_ISUID) != 0;
            lNode->next->ownR   = (buff.st_mode & S_IRUSR) != 0;
            lNode->next->ownW   = (buff.st_mode & S_IWUSR) != 0;
            lNode->next->ownX   = (buff.st_mode & S_IXUSR) != 0;
            lNode->next->groupR = (buff.st_mode & S_IRGRP) != 0;
            lNode->next->groupW = (buff.st_mode & S_IWGRP) != 0;
            lNode->next->groupX = (buff.st_mode & S_IXGRP) != 0;
            lNode->next->otherR = (buff.st_mode & S_IROTH) != 0;
            lNode->next->otherW = (buff.st_mode & S_IWOTH) != 0;
            lNode->next->otherX = (buff.st_mode & S_IXOTH) != 0;
        }
        
    }    
    return dirContent;
}

char* getFSMenuOption(fEntry* lHead) {

}

int main() {
    char *pwd = malloc(MAX_FOLDER_NAME);
    printf("fpMod starting in directory %s\n", pwd);
    
    // Get the current working directory
    if (getcwd(pwd, MAX_FOLDER_NAME) == NULL) {
        fprintf(stderr, "Error: Could not read the pwd with code %d\n", errno);
        exit(-1);
    }

    // Retrieve and list the files in the current directory
    fEntry* lsDirTest = listDir(pwd);
    while (lsDirTest != NULL) {
        printf(" - %s\n", lsDirTest->fname);
        printf("    - owner read: %d\n", lsDirTest->ownR);
        printf("    - owner write: %d\n", lsDirTest->ownW);
        printf("    - owner execute: %d\n", lsDirTest->ownX);
        printf("    - group read: %d\n", lsDirTest->groupR);
        printf("    - group write: %d\n", lsDirTest->groupW);
        printf("    - group execute: %d\n", lsDirTest->groupX);
        printf("    - other read: %d\n", lsDirTest->otherR);
        printf("    - other write: %d\n", lsDirTest->otherW);
        printf("    - other execute: %d\n", lsDirTest->otherX);
        printf("    - sticky: %d\n", lsDirTest->sticky);
        printf("    - SetUID: %d\n", lsDirTest->setUID);
        printf("    - SetGUID: %d\n", lsDirTest->setGID);


        lsDirTest = lsDirTest->next;
    }
}