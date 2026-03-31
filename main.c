#include <dialog.h>
#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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
        printf("Could not open directory %s", dir);
        exit(-1);
    }

    struct dirent* file;
    // Read each entry an create an fEntry for each
    while ((file = readdir(workingDir))) {
        struct stat buff;
        int status;

        status = stat(file->d_name, &buff);

        if (status == -1) {
            fprintf(stderr, "Error: failed to get status.");
            exit(-1);
        }

        // If the list is empty, allocate and record the file metadata
        if (dirContent == NULL) {
            dirContent = malloc(sizeof(fEntry));

            // Check for malloc errors
            if (dirContent == NULL) {
                fprintf(stderr, "Error: Failed to allocate memory for head of linked list\n");
                exit(-1);
            }
            // Record linked list data
            dirContent->len = 1;
            dirContent->next = NULL;
            // Record file metadata
            dirContent->fname = file->d_name;
            dirContent->isDir = (buff.st_mode & S_IFDIR);
            // Record file perms
            dirContent->sticky = status & S_ISVTX;
            dirContent->setGID = status & S_ISGID;
            dirContent->setUID = status & S_ISUID;
            dirContent->ownR = status & S_IRUSR;
            dirContent->ownW = status & S_IWUSR;
            dirContent->ownX = status & S_IXUSR;
            dirContent->groupR = status & S_IRGRP;
            dirContent->groupW = status & S_IWGRP;
            dirContent->groupX = status & S_IXGRP;
            dirContent->otherR = status & S_IROTH;
            dirContent->otherW = status & S_IWOTH;
            dirContent->otherX = status & S_IXOTH;
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
                fprintf(stderr, "Error: Failed to allocate memory for node of linked list\n");
                exit(-1);
            }
            // Record linked list data
            lNode->next->len = 1;
            lNode->next->next = NULL;
            // Record file metadata
            lNode->next->fname = file->d_name;
            lNode->next->isDir = (buff.st_mode & S_IFDIR);
            // Record file perms
            lNode->next->sticky = status & S_ISVTX;
            lNode->next->setGID = status & S_ISGID;
            lNode->next->setUID = status & S_ISUID;
            lNode->next->ownR = status & S_IRUSR;
            lNode->next->ownW = status & S_IWUSR;
            lNode->next->ownX = status & S_IXUSR;
            lNode->next->groupR = status & S_IRGRP;
            lNode->next->groupW = status & S_IWGRP;
            lNode->next->groupX = status & S_IXGRP;
            lNode->next->otherR = status & S_IROTH;
            lNode->next->otherW = status & S_IWOTH;
            lNode->next->otherX = status & S_IXOTH;
        }
        
    }    
    return dirContent;
}

char* getFSMenuOption(fEntry* lHead) {

}

int main() {
    fEntry* lsDirTest = listDir(".");
    while (lsDirTest != NULL) {
        printf(" - %s", lsDirTest->fname);
        lsDirTest = lsDirTest->next;
    }
}