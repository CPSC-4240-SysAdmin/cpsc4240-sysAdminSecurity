#include <dialog.h>
#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    bool sticky, setUID, setGID, ownR, ownW, ownX,
    groupR, groupW, groupX, otherR, otherW, otherX;
} fPerms;

typedef struct {
    // The file's permissions
    fPerms fperms;
    // The file's name
    char* fname;
    // If true, the file is a directory
    bool isDir;
} fEntry;

typedef struct {
    // Pointer to next item
    fsItem* next;
    // The file data
    fEntry file;
    // The length of the list
    int len;
} fsItem;

// Provides an linked list of all files and directories in pwd
fEntry* listDir(char* dir) {
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

        if ((buff.st_mode & S_IFREG) || (buff.st_mode & S_IFDIR)) {
            // Iterate over the head of the linked list
            fsItem* head = dirContent;
            while(head != NULL) {
                head->len++;
                head = head->next;
            }

            head = malloc(sizeof(fsItem));

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
    }

    return dirContent;
}

int main() {
    int status;
    char* options[] = {"op1", "op2", "op3", "op4", "op5"};
    init_dialog(stdin, stdout);
    status = dialog_menu(
        "fpMod",
        "choose a file/directory",
        0, 0, 25,
        5,
        options
    );
    
    end_dialog();

    return status;
}