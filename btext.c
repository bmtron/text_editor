#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>

#define BACKSPACE_KEY = 127
#define ESCAPE_KEY = 27
#define ENTER_KEY = 10


void get_user_input();
struct CursorPosition {
    int x;
    int y;
};

int main() {
    printf("Hello, World!\n");
    setlocale(LC_ALL, "");

    initscr();
    refresh();
    
    noecho();
    cbreak();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    WINDOW *mainwin = newwin(24, 80, 0, 0);
    wrefresh(mainwin);

    struct CursorPosition cursor;
    cursor.x = mainwin->_curx;
    cursor.y = mainwin->_cury;

    int curr_char;
    
    while ((curr_char = wgetch(mainwin))) {
        refresh();
        if (curr_char == 27) { //ESC key
            break;
        }
        if (curr_char == 127) { // BACKSPACE
            if (cursor.x > 0) {
                cursor.x--;
                waddch(mainwin, ' ');
                wmove(mainwin, cursor.y, cursor.x);
            }
            else {
                if (cursor.y > 0) {
                    cursor.y--;
                }
                wmove(mainwin, cursor.y, cursor.x);
            }
            refresh();
            continue;
        }
        if (curr_char == 10) {
            waddch(mainwin, '\n');
            cursor.y++;
            cursor.x = 0;
            wmove(mainwin, cursor.y, cursor.x);
            continue;
        }
        waddch(mainwin, curr_char);
        cursor.x++;
        wmove(mainwin, cursor.y, cursor.x);
        refresh();
    }
    refresh();
    delwin(mainwin);
    

    endwin();
    return 0;
}

void get_user_input() {
    char input[100];
    printf("Enter your name: ");
    scanf("%s", input);
    printf("Hello, %s\n", input);
}
