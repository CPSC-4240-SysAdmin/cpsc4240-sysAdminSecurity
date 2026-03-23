#include <dialog.h>
#include <ncurses.h>

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