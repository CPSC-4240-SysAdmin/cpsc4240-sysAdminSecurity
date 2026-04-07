#include <dialog.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_FOLDER_NAME 255
#define PERMISSION_LEN 11

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
            // Record file metadata (copy name; file->d_name is reused by readdir)
            dirContent->fname = strdup(file->d_name);
            if (dirContent->fname == NULL) {
                fprintf(stderr, "Error: strdup failed for file name\n");
                exit(-1);
            }
            dirContent->isDir = S_ISDIR(buff.st_mode);
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
            lNode->len++;
            lNode->next = malloc(sizeof(fEntry));

            // Check for malloc errors
            if (lNode->next == NULL) {
                fprintf(stderr, "Error: Failed to allocate memory for node of linked list with code %d\n", errno);
                exit(-1);
            }
            // Record linked list data
            lNode->next->len = 1;
            lNode->next->next = NULL;
            // Record file metadata (copy name; file->d_name is reused by readdir)
            lNode->next->fname = strdup(file->d_name);
            if (lNode->next->fname == NULL) {
                fprintf(stderr, "Error: strdup failed for file name\n");
                exit(-1);
            }
            lNode->next->isDir = S_ISDIR(buff.st_mode);
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

// returns string of symbolic permissions of the particular file f
// Ex. drwxr--r-- means its directory with owner rwx and g and other only r
// for the setUID and sticky representation
// see https://en.wikipedia.org/wiki/File-system_permissions#Symbolic_notation
// for better explanation.
char* getFilePermisionsString(fEntry f){
    static char permissions[PERMISSION_LEN];

    memcpy(permissions, "----------\0", sizeof(permissions));
    if (f.isDir){
        permissions[0] = 'd';
    }
    if (f.ownR){
        permissions[1] = 'r';
    }
    if (f.ownW){
        permissions[2] = 'w';
    }
    if (f.ownX && !f.setUID){
        permissions[3] = 'x';
    }
    if (f.ownX && f.setUID){
        permissions[3] = 's';
    }
    if (!f.ownX && f.setUID){
        permissions[3] = 'S';
    }
    if (f.groupR){
        permissions[4] = 'r';
    }
    if (f.groupW){
        permissions[5] = 'w';
    }
    if (f.groupX && !f.setGID){
        permissions[6] = 'x';
    }
    if (f.groupX && f.setGID){
        permissions[6] = 's';
    }
    if (!f.groupX && f.setGID){
        permissions[6] = 'S';
    }
    if (f.otherR){
        permissions[7] = 'r';
    }
    if (f.otherW){
        permissions[8] = 'w';
    }
    if (f.otherX && !f.sticky){
        permissions[9] = 'x';
    }
    if (f.otherX && f.sticky){
        permissions[9] = 't';
    }
    if (!f.otherX && f.sticky){
        permissions[9] = 'T';
    }
    return permissions;

}

// Returns string Format of 
// "file 1 name", "file 1 permissions", ... so on n times
// n is amount of files in list
char** getFSMenuOption(fEntry* lHead) {
    
    int numFiles = 0;//lsDirTest->len;
    for (fEntry* p = lHead; p != NULL; p = p->next) {
        numFiles++;
    }
    char** menuOpts = malloc(2 * numFiles * sizeof(char*));
    if (menuOpts == NULL && numFiles > 0) {
        fprintf(stderr, "Error: malloc failed for menu options\n");
        exit(-1);
    }
    int i = 0; 
    fEntry *tmp = lHead;
    while (tmp != NULL){
        char fileName[MAX_FOLDER_NAME + 1]; 
           
        sprintf(fileName, "%s", tmp->fname); 
        menuOpts[i * 2] = strdup(fileName);
        menuOpts[i * 2 + 1] = strdup(getFilePermisionsString(*tmp));
        i++;
        tmp = tmp->next;
    }
    return menuOpts;
}

char* selectFile(char** menuOpts, int menuSize){
    int status;
    dialog_state.use_colors = 1;
	init_dialog(stdin, stdout);
    tag_key_attr = tag_attr; 
    tag_key_selected_attr = tag_selected_attr;
	status = dialog_menu(
			"File Selection",
			"Please Select File or Directory to Change Permissions",
			0, 
            0,
            0,
            menuSize,
            menuOpts
    );
	end_dialog();
    // Outupt of the menu
    return dialog_vars.input_result;

}

//TODO: 
// Get file name from fEntry pwd list of files.
// from the filename string
fEntry* getFEntryFromString(char* fileName){
    return NULL;
}


//TODO:
//create a dialog checklist that shows indivudal seperate permissions
    /*
    *
    *ur                 []
    *uw                 [*]
    *ux                 []
    *gr                 [*]
    *special Permissions[]
    */
// Make sure it actually changes the file permissions
void checklistPermissions(fEntry* file){
    
}

//TODO:
// Go into the directroy of the file
// Maybe check if it is dir but 
void goIntoDir(fEntry* file){


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
    // Gathering file names in menu string array format
    // Need size for the menu
    char** menuOpts = getFSMenuOption(lsDirTest);
    int menuSize = sizeof(menuOpts);

    char* outputFile = selectFile(menuOpts, menuSize);


    ///THE FLOW
    //fEntry selectedFile = getFEntryFromString(outputFile);
    /*
    while(cancel not selected){
        fEntry* lsHead = lsDir(pwd)
        char** menuOpts = getFSMenuOption(lsHead);
        int menuSize = sizeof(menuOpts);
        char* fileName = selectFile(menuOpts, menuSize); //creates the menu
        fEntry selectedFile = getFEntryFromString(fileName);
        if (selectedFile->isDir && selectedFile->fname != "."){
            //Enter Directory
            // goIntoDir(nameOfDir)
        }
        else{
            // Permissions checklist
            // checklistPermissions(file)
        }
        free(BOTH fEntry and menuOpts)
    }
    */
    

    for (int y = 0; y < 2 * menuSize; y++) {
        free(menuOpts[y]);
    }
    free(menuOpts);
    
}
