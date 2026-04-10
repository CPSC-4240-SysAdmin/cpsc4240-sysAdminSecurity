#include <dialog.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_FOLDER_NAME 256 //The maximum length a name can be for a file or directory +1
#define MAX_PATH_LENGTH 4096
#define PERMISSION_LEN 12

int dialog_status;

typedef enum {
    MENU_STATE,
    CHECKLIST_STATE,
    QUIT_STATE
} DialogState;


// Is a linked list of all hidden files and directories within a 
// particular directory. 
// Stores the name of the file, if its a directory and other permissions about it
typedef struct fEntry {
    /* The File data */

    // The file's name
    char fname[MAX_FOLDER_NAME];

    // If true, the file is a directory
    bool isDir;
    
    // The file's permissions
    bool sticky, setUID, setGID, ownR, ownW, ownX,
    groupR, groupW, groupX, otherR, otherW, otherX;

    // The next item in the list
    struct fEntry* next;

} fEntry;


// Stores the pointer to the linked list of 
// files within the directory along with the name of the directory
// and amount of files/subdirectories within it (minus the "." and ".." dir)
// (Will include all hidden files and subdirectories)
typedef struct dirEntry {
    //Directory name
    char dirName[MAX_FOLDER_NAME];

    // num of ALL files/subdirs
    int len;
    
    // Pointer to start of fEntry linked list
    struct fEntry* start;


} dirEntry;


// Global current directory 
dirEntry currentDir;


//Takes a path and returns the end most folder (or file)
//Ex:
// getCurrentFolder(/Users/alex/Desktop/work) returns work
char* getCurrentFolder(char* folderPath){
    char* currentFolder = (char*)malloc(sizeof(char)*MAX_FOLDER_NAME);
    if (currentFolder == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for head of linked list with code %d\n", errno);
        exit(-1);
    }
    currentFolder = strrchr(folderPath, '/');
    if (currentFolder != NULL) {
        return currentFolder + 1;
    }
    return currentFolder; 
}


// Free's the fEntry Linked list in current Dir
void freeDirEntry() {
    fEntry *temp = currentDir.start;
    while (currentDir.start != NULL) {
        temp = currentDir.start;
        currentDir.start = currentDir.start->next;
        free(temp);
    }
    currentDir.len = 0;
}

// Updates the currentDir based on the pwd 
// pwd is a string of the absoloute path
void updateDirEntry(char* pwd) {

    // Opening current directory
    DIR* workingDir = opendir(pwd);
    // Declaring struct to point to files/dirs within directory
    struct dirent* file;


    // Free the fEntry linked list that dirEntry points to to make room for a new one
    freeDirEntry(); 

    // Initializing to an empty Current directory
    strncpy(currentDir.dirName, getCurrentFolder(pwd), MAX_FOLDER_NAME);
    fEntry* dirContent = NULL;


    // Our traversal node to append more nodes
    fEntry* currentFile;

    // Checking if we have permisions to open 
    if (workingDir == NULL) {
        if (errno == EACCES) {
            printf("Error: Permission denied!\n");
            exit(-1);
        } else if (errno == ENOENT) {
            printf("Error: Directory does not exist!\n");
            exit(-1);
        }
    }


    // Read each entry an create an fEntry for each
    while ((file = readdir(workingDir))) {

        // The struct that contains the permissions data on the file
        struct stat buff;

        // Skips recording the current "." and parent ".." directories
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue; 
        } 
        // Records data on the specific file/dir 
        if (stat(file->d_name, &buff) == 0) {
            
            // First element in our Linked List
            if (dirContent == NULL) {
                dirContent = (fEntry*)malloc(sizeof(fEntry)); 
                if (dirContent == NULL) {
                    fprintf(stderr, "Error: Failed to allocate memory for head of linked list with code %d\n", errno);
                    exit(-1);
                }
                // Record file/dir name
                strncpy(dirContent->fname, file->d_name, MAX_FOLDER_NAME);
                dirContent->fname[MAX_FOLDER_NAME-1] = '\0';

                // Record rest of the data
                dirContent->next = NULL;
                dirContent->isDir = S_ISDIR(buff.st_mode);
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
                
                currentDir.len++;
                currentFile = dirContent;
            }

            // Now we add any remaining files left in dir to our LinkList 
            else {
                fEntry* newFile = (fEntry*)malloc(sizeof(fEntry));
                if (newFile == NULL) {
                    fprintf(stderr, "Error: Failed to allocate memory for head of linked list with code %d\n", errno);
                    exit(-1);
                }

                // Record file/dir name
                strncpy(newFile->fname, file->d_name, MAX_FOLDER_NAME);
                newFile->fname[MAX_FOLDER_NAME-1] = '\0';

                // Record rest of the data
                newFile->next = NULL;
                newFile->isDir = S_ISDIR(buff.st_mode);
                newFile->sticky = (buff.st_mode & S_ISVTX) != 0;
                newFile->setGID = (buff.st_mode & S_ISGID) != 0;
                newFile->setUID = (buff.st_mode & S_ISUID) != 0;
                newFile->ownR = (buff.st_mode & S_IRUSR) != 0;
                newFile->ownW = (buff.st_mode & S_IWUSR) != 0;
                newFile->ownX = (buff.st_mode & S_IXUSR) != 0;
                newFile->groupR = (buff.st_mode & S_IRGRP) != 0;
                newFile->groupW = (buff.st_mode & S_IWGRP) != 0;
                newFile->groupX = (buff.st_mode & S_IXGRP) != 0;
                newFile->otherR = (buff.st_mode & S_IROTH) != 0;
                newFile->otherW = (buff.st_mode & S_IWOTH) != 0;
                newFile->otherX = (buff.st_mode & S_IXOTH) != 0;

                currentDir.len++;
                currentFile->next = newFile;
                currentFile = newFile;
            }
        }
        
    }    
    currentDir.start = dirContent;
    closedir(workingDir);
}

// Frees memory allocated by the fEntry linked list within our currentDir

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
char** getFSMenuOption() {
    char** menuOpts = (char**)malloc(2 * currentDir.len * sizeof(char*));
    if (menuOpts == NULL && currentDir.len > 0) {
        fprintf(stderr, "Error: malloc failed for menu options\n");
        exit(-1);
    }
    int i = 0; 
    fEntry *tmp = currentDir.start;
    while (tmp != NULL){
        menuOpts[i * 2] = strdup(tmp->fname);
        menuOpts[i * 2 + 1] = strdup(getFilePermisionsString(*tmp));
        i++;
        tmp = tmp->next;
    }
    return menuOpts;
}

void selectFile(char** menuOpts, char* folderName){
    
    char folderBuf[MAX_FOLDER_NAME];
    snprintf(folderBuf, sizeof(folderBuf), "Select file or directory from\n%s", folderName);
    dlg_clr_result();
	dialog_status = dialog_menu(
			"Selection",
			folderBuf,
			0, 
            0,
            0,
            currentDir.len,
            menuOpts
    );
//	end_dialog();

}

// Returns an fEntry in list dirContent whose fname matches fileName
fEntry* getFEntryFromString(char* fileName){

    fEntry* head = currentDir.start;
    while (head != NULL) {
        if (strcmp(head->fname, fileName) == 0) {
            return head;
        }
        head = head->next;
    }

    return NULL;
}


//TODO:
//create a dialog checklist that shows indivudal seperate permissions
    /*
    *
    *ur                 []
    *uw                 [*]
    *ux                 []f
    *gr                 [*]
    *special Permissions[]
    */
// Make sure it actually changes the file permissions
// returns status of checklist (ok 0, cancel 1, or espace 255)
void checklistPermissions(fEntry* file){

    char* permList[] = {
        "OR", "Owner Read", file->ownR ? "on" : "off",
        "OW", "Owner Write", file->ownW ? "on" : "off",
        "OX", "Owner Execute", file->ownX ? "on" : "off",
        "GR", "Group Read", file->groupR ? "on" : "off",
        "GW", "Group Write", file->groupW ? "on" : "off",
        "GX", "Group Execute", file->groupX ? "on" : "off",
        "TR", "Other Read", file->otherR ? "on" : "off",
        "TW", "Other Write", file->otherW ? "on" : "off",
        "TX", "Other Execute", file->otherX ? "on" : "off",
        "SU", "SetUID", file->setUID ? "on" : "off",
        "SG", "SetGID", file->setGID ? "on" : "off",
        "ST", "Sticky", file->sticky ? "on" : "off"
    };
    // Ask the user via checkbox dialog to set file perms they want

    char dialogTitle[MAX_FOLDER_NAME + 24];
    snprintf(dialogTitle, MAX_FOLDER_NAME + 24, "Setting Permissions for %s", file->fname);

    dlg_clr_result();
	dialog_status = dialog_checklist(
			dialogTitle,
			"Navigate list: Up and Down Arrows\n Enable/Disable Permission: Space",
			0, 
            0,
            12,
            PERMISSION_LEN,
            permList,
            FLAG_CHECK
    );
//	end_dialog();

    // Set the file's permissions to match their choices
        
    if (dialog_status == 0) { // Selected OK 
        mode_t newPerms = 0;
        // Set owner
        newPerms += strstr(dialog_vars.input_result, "OR") ? S_IRUSR : 0; 
        newPerms += strstr(dialog_vars.input_result, "OW") ? S_IWUSR : 0; 
        newPerms += strstr(dialog_vars.input_result, "OX") ? S_IXUSR : 0; 
        // Set group
        newPerms += strstr(dialog_vars.input_result, "GR") ? S_IRGRP : 0; 
        newPerms += strstr(dialog_vars.input_result, "GW") ? S_IWGRP : 0; 
        newPerms += strstr(dialog_vars.input_result, "GX") ? S_IXGRP : 0; 
        // Set other
        newPerms += strstr(dialog_vars.input_result, "TR") ? S_IROTH : 0; 
        newPerms += strstr(dialog_vars.input_result, "TW") ? S_IWOTH : 0; 
        newPerms += strstr(dialog_vars.input_result, "TX") ? S_IXOTH : 0; 
        // Set special
        newPerms += strstr(dialog_vars.input_result, "SU") ? S_ISUID : 0; 
        newPerms += strstr(dialog_vars.input_result, "SG") ? S_ISGID : 0; 
        newPerms += strstr(dialog_vars.input_result, "ST") ? S_ISVTX : 0;

        // What actually changes file permission within your system
        chmod(file->fname, newPerms);
    }

}



void freeMenuOpts(char** menuOpts){
    for (int i = 0; i < sizeof(menuOpts); i++){
        free(menuOpts[i]);
    }
    free(menuOpts);
}



int main() {
    
    char *pwd = malloc(MAX_PATH_LENGTH);
    if (getcwd(pwd, MAX_PATH_LENGTH) == NULL) {
        fprintf(stderr, "Error: Could not read the pwd with code %d\n", errno);
        exit(-1);
    }
    updateDirEntry(pwd);
    char** menuOpts;
    char* folderName = getCurrentFolder(pwd);
    fEntry* selectedFile;

    init_dialog(stdin, stdout);
    DialogState currentState = MENU_STATE;
    dialog_state.use_colors = 1;
    tag_key_attr = tag_attr; 
    tag_key_selected_attr = tag_selected_attr;

    while (currentState != QUIT_STATE){


        dlg_clear();
        switch (currentState) {

            case MENU_STATE: {
                menuOpts = getFSMenuOption();
                selectFile(menuOpts, folderName);
                if (dialog_status == 0 ) { // Selected OK
                    selectedFile = getFEntryFromString(dialog_vars.input_result); 
                    currentState = CHECKLIST_STATE;
                }
                else { // Selected CANCEL
                    currentState = QUIT_STATE;
                }
                break;
            }

            case CHECKLIST_STATE: {
                checklistPermissions(selectedFile);
                if (dialog_status == 0) { // Selected OK
                   currentState = MENU_STATE; 
                   updateDirEntry(pwd);
                }
                else { // Selected CANCEL
                    currentState = MENU_STATE;
                }
                break;
            }
            default: {
                currentState = QUIT_STATE;
                break;
            }
        }
    }
    end_dialog();
    //free(folderName);
    return 0;

}
