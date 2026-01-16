#include "vite.h"

/* Display Functions */
void print_from_row(EditorState* state, int start_row, int end_row) {
    Node* current = state->head;
    
    // Skip head (row 0) and move to start_row
    // start_row is 0-based for display purposes
    for (int i = 0; i <= start_row && current != NULL; i++) {
        if (i < start_row) {
            current = current->next;
        }
    }
    
    // Print rows until end_row
    while (current != NULL && current != state->head) {
        if (current->row > end_row) break;
        printf("%s\n", current->data != NULL ? current->data : "");
        current = current->next;
    }
}

void print_all(EditorState* state) {
    Node* current = state->head;
    while (current != NULL && current->next != NULL) {
        current = current->next;
        printf("%s\n", current->data != NULL ? current->data : "");
    }
}

void highlight_text(const char* text, const char* pattern) {
    const char* found = strstr(text, pattern);
    if (found != NULL) {
        fwrite(text, 1, found - text, stdout);
        printf("\033[7m%s\033[27m", pattern);
        highlight_text(found + strlen(pattern), pattern);
    } else {
        printf("%s", text);
    }
}

void draw_status_bar(EditorState* state) {
    int total_lines = get_total_lines(state);
    int current_line = state->cursor_y + state->page_offset_y - 1;
    
    // Ensure current_line is at least 1
    if (current_line < 1) current_line = 1;
    if (total_lines < 1) total_lines = 1;
    
    int line_digits = count_digits(total_lines);
    int current_digits = count_digits(current_line);
    int total_digits = count_digits(total_lines);
    
    int filename_len = state->has_filename ? strlen(state->filename) : 0;
    int status_len = 19 + line_digits + current_digits + total_digits + filename_len;
    
    move_cursor(1, state->row_count);
    clear_line();
    
    printf("\033[7m");
    if (state->has_filename) {
        printf("[%s] - %d lines", state->filename, total_lines);
    } else {
        printf("[No Name] - %d lines", total_lines);
    }
    
    for (int i = 0; i < state->col_count - status_len; i++) {
        printf(" ");
    }
    printf("no ft | %d/%d", current_line, total_lines);
    printf("\033[27m\n");
}

void draw_help_bar(EditorState* state) {
    move_cursor(1, state->row_count + 1);
    clear_line();
    printf("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
}

void initialize_screen(EditorState* state) {
    system(CLEAR_CMD);
    disable_wrap();
    
    for (int i = 0; i < state->row_count - 1; i++) {
        printf("~\n");
    }
    
    draw_status_bar(state);
    draw_help_bar(state);
    move_cursor(1, 1);
}

void update_status_bar(EditorState* state) {
    draw_status_bar(state);
    draw_help_bar(state);
}

/* Refresh the visible screen area */
void refresh_screen(EditorState* state) {
    // Clear the editing area (excluding status and help bars)
    for (int i = 1; i < state->row_count; i++) {
        move_cursor(1, i);
        clear_line();
    }
    
    // Print visible rows starting from page_offset_y
    // page_offset_y represents how many rows we've scrolled down
    Node* current = state->head;
    int start_row = state->page_offset_y;
    
    // Skip head (row 0) and move to start_row (1-based)
    for (int i = 0; i <= start_row && current != NULL; i++) {
        if (i < start_row) {
            current = current->next;
        }
    }
    
    int screen_row = 1;
    int max_rows = state->row_count - 1; // Exclude status bar
    
    // Print visible rows
    while (current != NULL && current != state->head && screen_row <= max_rows) {
        move_cursor(1, screen_row);
        if (current->data != NULL) {
            printf("%s", current->data);
        }
        current = current->next;
        screen_row++;
    }
    
    // Fill remaining lines with ~
    while (screen_row <= max_rows) {
        move_cursor(1, screen_row);
        printf("~");
        screen_row++;
    }
}
