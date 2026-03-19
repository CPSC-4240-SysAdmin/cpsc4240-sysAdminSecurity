#include <dialog.h>
#include <ncurses.h>

int main() {
    int status;
    init_dialog(stdin, stdout);
    status = dialog_yesno(
                "Hello, in dialog-format",
                "Hello World!",
                0, 0);
    end_dialog();
    return status;
}