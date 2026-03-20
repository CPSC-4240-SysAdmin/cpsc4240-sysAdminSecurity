#include <dialog.h>
#include <ncurses.h>

int main() {
    int status;
    init_dialog(stdin, stdout);
    status = dialog_fselect(
        "fpMod",
        "",
        0, 0
    );
    
    end_dialog();
    return status;
}