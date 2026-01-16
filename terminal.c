#include "vite.h"

/* Terminal Control Functions */
void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void clear_line(void) {
    printf("\033[2K");
}

int getch(void) {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_iflag &= ~(IXON);
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

int disable_wrap(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return FALSE;
    }
    dwMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
    if (!SetConsoleMode(hOut, dwMode)) {
        return FALSE;
    }
#else
    if (isatty(fileno(stdout))) {
        printf("\e[?7l");
    }
#endif
    return TRUE;
}

void get_terminal_size(EditorState* state) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo)) {
        state->row_count = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top;
        state->col_count = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left;
    } else {
        state->row_count = 24;
        state->col_count = 80;
    }
#else
    struct winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) {
        state->row_count = size.ws_row - 1;
        state->col_count = size.ws_col - 1;
    } else {
        state->row_count = 24;
        state->col_count = 80;
    }
#endif
}
