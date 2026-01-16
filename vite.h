#ifndef VITE_H
#define VITE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#define UP 72
#define DOWN 80
#define RIGHT 77
#define LEFT 75
#define BACKSPACE 8
#define HOME 71
#define END 79
#define PAGEUP 73
#define PAGEDOWN 81
#define ENTER 13
#define CTRL_S 19
#define CTRL_Q 17
#define CTRL_F 6
#define CLEAR_CMD "cls"
#define ARROW_PREFIX_1 224
#define ARROW_PREFIX_2 0
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#define UP 65
#define DOWN 66
#define RIGHT 67
#define LEFT 68
#define BACKSPACE 127
#define HOME 72
#define END 70
#define PAGEUP 53
#define PAGEDOWN 54
#define ENTER 10
#define CTRL_S 19
#define CTRL_Q 17
#define CTRL_F 6
#define CLEAR_CMD "clear"
#define ESCAPE 27
#endif

#define TRUE 1
#define FALSE 0
#define MAX_FILENAME 100
#define MAX_WORD_LENGTH 100
#define MAX_LINE_LENGTH 100000
#define MIN_PRINTABLE_CHAR 32
#define MAX_PRINTABLE_CHAR 126

/* Data Structures */
typedef struct Node {
    char* data;
    int row;
    int size;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct {
    Node* head;
    int cursor_x;
    int cursor_y;
    int page_offset_y;
    int row_count;
    int col_count;
    char filename[MAX_FILENAME];
    int has_filename;
} EditorState;

/* Terminal Control Functions */
int getch(void);
void move_cursor(int x, int y);
void clear_line(void);
int disable_wrap(void);
void get_terminal_size(EditorState* state);

/* Utility Functions */
int count_digits(int num);
int get_total_lines(EditorState* state);
Node* get_current_row(EditorState* state);

/* Node Management Functions */
void init_node_list(EditorState* state);
Node* create_node(const char* data, int size, int row);

/* Text Editing Functions */
void insert_char_at_position(EditorState* state, char ch);
void insert_newline(EditorState* state);
void delete_char_at_cursor(EditorState* state);

/* Display Functions */
void print_from_row(EditorState* state, int start_row, int end_row);
void print_all(EditorState* state);
void highlight_text(const char* text, const char* pattern);
void draw_status_bar(EditorState* state);
void draw_help_bar(EditorState* state);
void initialize_screen(EditorState* state);
void update_status_bar(EditorState* state);
void refresh_screen(EditorState* state);

/* File I/O Functions */
int read_file(EditorState* state, const char* filename);
void save_file(EditorState* state, const char* filename);

/* Input Handling Functions */
void handle_char_input(EditorState* state, char ch);
void handle_backspace(EditorState* state);
void handle_enter(EditorState* state);
void handle_save(EditorState* state);
void handle_find(EditorState* state);
void handle_arrow_keys(EditorState* state, int key);

/* Editor State Management */
void init_editor_state(EditorState* state);
void free_editor_state(EditorState* state);
void run_editor(EditorState* state);

#endif /* VITE_H */
