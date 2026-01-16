#include "vite.h"

/* File I/O Functions */
int read_file(EditorState* state, const char* filename) {
    FILE* file = fopen(filename, "rt");
    if (file == NULL) {
        return FALSE;
    }
    
    char buffer[MAX_LINE_LENGTH];
    int row = 1;
    int has_content = FALSE;
    
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        has_content = TRUE;
        int len = strlen(buffer);
        
        // Remove newline
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[--len] = '\0';
        }
        
        Node* new_row = create_node(buffer, len, row);
        if (new_row == NULL) {
            fclose(file);
            return FALSE;
        }
        
        if (state->head->next == NULL) {
            state->head->next = new_row;
            new_row->prev = state->head;
        } else {
            Node* current = state->head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_row;
            new_row->prev = current;
        }
        
        row++;
    }
    
    // If file is empty, create at least one empty row
    if (!has_content) {
        Node* new_row = create_node(NULL, 0, 1);
        if (new_row != NULL) {
            state->head->next = new_row;
            new_row->prev = state->head;
        }
    }
    
    fclose(file);
    return TRUE;
}

void save_file(EditorState* state, const char* filename) {
    FILE* file = fopen(filename, "wt");
    if (file == NULL) return;
    
    Node* current = state->head;
    while (current != NULL && current->next != NULL) {
        current = current->next;
        if (current->data != NULL) {
            fputs(current->data, file);
        }
        if (current->next != NULL) {
            fputc('\n', file);
        }
    }
    
    fclose(file);
}

void handle_save(EditorState* state) {
    if (!state->has_filename) {
        move_cursor(1, state->row_count + 1);
        clear_line();
        printf("file name : ");
        char filename[MAX_FILENAME];
        if (scanf("%99s", filename) == 1) {
            strncpy(state->filename, filename, MAX_FILENAME - 1);
            state->filename[MAX_FILENAME - 1] = '\0';
            state->has_filename = TRUE;
        }
    }
    
    system(CLEAR_CMD);
    print_all(state);
    save_file(state, state->filename);
    
    draw_status_bar(state);
    draw_help_bar(state);
    
    move_cursor(1, state->row_count + 1);
    clear_line();
    printf("file save!");
    move_cursor(state->cursor_x, state->cursor_y);
}

void handle_find(EditorState* state) {
    char word[MAX_WORD_LENGTH];
    move_cursor(1, state->row_count + 1);
    clear_line();
    printf("find word : ");
    fflush(stdout);
    
    if (scanf("%99s", word) == 1) {
        system(CLEAR_CMD);
        disable_wrap();
        
        // Print all visible rows with highlighting
        Node* current = state->head;
        int start_row = state->page_offset_y;
        int max_rows = state->row_count - 2; // Exclude status and help bars
        
        // Skip head (row 0) and move to start_row (1-based)
        for (int i = 0; i <= start_row && current != NULL; i++) {
            if (i < start_row) {
                current = current->next;
            }
        }
        
        int screen_row = 1;
        while (current != NULL && current != state->head && screen_row <= max_rows) {
            if (current->data != NULL) {
                highlight_text(current->data, word);
            }
            printf("\n");
            current = current->next;
            screen_row++;
        }
        
        // Fill remaining lines with ~
        while (screen_row <= max_rows) {
            printf("~\n");
            screen_row++;
        }
        
        update_status_bar(state);
        draw_help_bar(state);
        move_cursor(state->cursor_x, state->cursor_y);
    }
}
