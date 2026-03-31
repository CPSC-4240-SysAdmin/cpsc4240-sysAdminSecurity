#include <dialog.h>
#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct fPerms {
    bool sticky, setUID, setGID, ownR, ownW, ownX,
    groupR, groupW, groupX, otherR, otherW, otherX;
} fPerms;

typedef struct fEntry {
    // The file's permissions
    fPerms fperms;
    // The file's name
    char* fname;
    // If true, the file is a directory
    bool isDir;
} fEntry;

typedef struct fsItem {
    // Pointer to next item
    struct fsItem* next;
    // The file data
    fEntry file;
    // The length of the list from the current item to the last
    int len;
} fsItem;

// Provides an linked list of all files and directories in pwd
fsItem* listDir(char* dir) {
    fsItem* dirContent = NULL;
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
        
        fsItem* head = dirContent;
        // If empty allocate first item
        if (head == NULL) {
            dirContent = malloc(sizeof(fsItem));
            dirContent->len = 1;
            dirContent->next = NULL;
        }
        // If not empty, walk to the end of the list and allocate 
        else {
            while(head->next != NULL) {
                head->len++;
                head = head->next;
            }

            head->next = malloc(sizeof(fsItem));
            head = head->next;
            head->next = NULL;
        }

        if (head == NULL) {
            fprintf(stderr, "Error: malloc call to build the file list failed");
            exit(-1);
            
        }

        // Assign its file name
        head->file.fname = file->d_name;
        // Check if its a directory
        head->file.isDir = (buff.st_mode & S_IFDIR);
        // Obtain and store the file permissions
        head->file.fperms.sticky = status & S_ISVTX;
        head->file.fperms.setGID = status & S_ISGID;
        head->file.fperms.setUID = status & S_ISUID;
        head->file.fperms.ownR = status & S_IRUSR;
        head->file.fperms.ownW = status & S_IWUSR;
        head->file.fperms.ownX = status & S_IXUSR;
        head->file.fperms.groupR = status & S_IRGRP;
        head->file.fperms.groupW = status & S_IWGRP;
        head->file.fperms.groupX = status & S_IXGRP;
        head->file.fperms.otherR = status & S_IROTH;
        head->file.fperms.otherW = status & S_IWOTH;
        head->file.fperms.otherX = status & S_IXOTH;
        
    }

    
    return dirContent;
}

char* getFSMenuOption(fsItem* lHead) {

}

int main() {
    fsItem* lsDirTest = listDir(".");
    while (lsDirTest != NULL) {
        printf(" - %s", lsDirTest->file.fname);
        lsDirTest = lsDirTest->next;
    }
}