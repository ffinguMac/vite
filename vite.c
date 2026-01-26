#include "vite.h"

/* Main Event Loop */
void run_editor(EditorState* state) {
    int exit_flag = FALSE;
    
    while (1) {
        int ch = getch();
        
        if (ch >= MIN_PRINTABLE_CHAR && ch <= MAX_PRINTABLE_CHAR) {
            handle_char_input(state, ch);
        } else if (ch == BACKSPACE) {
            handle_backspace(state);
        } else if (ch == ENTER) {
            handle_enter(state);
        } else if (ch == CTRL_S) {
            handle_save(state);
        } else if (ch == CTRL_Q) {
            if (exit_flag) {
                move_cursor(1, state->row_count + 1);
                clear_line();
                printf("exit!!");
                return;
            }
            exit_flag = TRUE;
            continue;
        } else if (ch == CTRL_F) {
            handle_find(state);
        }
#ifdef _WIN32
        else if (ch == ARROW_PREFIX_1 || ch == ARROW_PREFIX_2) {
            int arrow_key = getch();
            handle_arrow_keys(state, arrow_key);
        }
#else
        else if (ch == ESCAPE) {
            int next_ch = getch();
            if (next_ch == '[') {
                int arrow_key = getch();
                // Check for PageUp/PageDown (5~ or 6~)
                if (arrow_key == '5' || arrow_key == '6') {
                    int tilde = getch();
                    if (tilde == '~') {
                        if (arrow_key == '5') {
                            handle_arrow_keys(state, PAGEUP);
                        } else if (arrow_key == '6') {
                            handle_arrow_keys(state, PAGEDOWN);
                        }
                    }
                } else {
                    handle_arrow_keys(state, arrow_key);
                }
            } else if (next_ch == '0') {
                int key = getch();
                if (key == HOME || key == END) {
                    handle_arrow_keys(state, key);
                }
            }
        }
#endif
        else {
            exit_flag = FALSE;
        }
        
        // Update status bar and cursor position
        update_status_bar(state);
        move_cursor(state->cursor_x, state->cursor_y);
    }
}

/* Main Function */
int main(int argc, char** argv) {
    EditorState state;
    
    init_editor_state(&state);
    get_terminal_size(&state);
    
    if (argc == 2) {
        // Open existing file
        if (!read_file(&state, argv[1])) {
            fprintf(stderr, "Error: Could not open file '%s'\n", argv[1]);
            free_editor_state(&state);
            return 1;
        }
        strncpy(state.filename, argv[1], MAX_FILENAME - 1);
        state.filename[MAX_FILENAME - 1] = '\0';
        state.has_filename = TRUE;
        
        initialize_screen(&state);
        refresh_screen(&state);
        state.cursor_y = 1;
        state.cursor_x = 1;
        update_status_bar(&state);
        move_cursor(1, 1);
    } else {
        // New file
        initialize_screen(&state);
    }
    
    run_editor(&state);
    
    free_editor_state(&state);
    return 0;
}
