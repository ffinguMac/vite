#include "vite.h"

/* Utility Functions */
int count_digits(int num) {
    if (num == 0) return 1;
    int count = 0;
    while (num > 0) {
        num /= 10;
        count++;
    }
    return count;
}

int get_total_lines(EditorState* state) {
    Node* current = state->head;
    int count = 0;
    while (current != NULL && current->next != NULL) {
        current = current->next;
        count = current->row;
    }
    // If no lines exist, return 0 (but we should have at least one empty line)
    return count > 0 ? count : 0;
}

Node* get_current_row(EditorState* state) {
    Node* current = state->head;
    int target_row = state->cursor_y + state->page_offset_y;
    
    // Ensure target_row is at least 1 (first actual row is row 1)
    if (target_row < 1) target_row = 1;
    
    // Skip head (row 0) and move to target_row
    for (int i = 0; i < target_row && current != NULL; i++) {
        current = current->next;
        if (current == NULL) break;
    }
    
    // If we couldn't reach target_row, return the last row
    if (current == NULL || current == state->head) {
        current = state->head;
        while (current != NULL && current->next != NULL) {
            current = current->next;
        }
    }
    
    return current;
}

/* Node Management Functions */
void init_node_list(EditorState* state) {
    state->head = (Node*)malloc(sizeof(Node));
    if (state->head == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    state->head->next = NULL;
    state->head->prev = NULL;
    state->head->data = NULL;
    state->head->row = 0;
    state->head->size = 0;
}

Node* create_node(const char* data, int size, int row) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) return NULL;
    
    node->size = size;
    node->data = (char*)malloc((size + 1) * sizeof(char));
    if (node->data == NULL) {
        free(node);
        return NULL;
    }
    
    if (data != NULL && size > 0) {
        memcpy(node->data, data, size);
    }
    node->data[size] = '\0';
    node->row = row;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

/* Text Insertion Functions */
void insert_char_at_position(EditorState* state, char ch) {
    Node* current = get_current_row(state);
    
    if (current == NULL || current == state->head) {
        // First input - create first row
        Node* new_row = create_node(&ch, 1, 1);
        if (new_row == NULL) return;
        
        new_row->next = NULL;
        new_row->prev = state->head;
        state->head->next = new_row;
        printf("%c", ch);
        return;
    }
    
    int pos = state->cursor_x - 1;
    
    // Check if current line length has reached the screen width
    // If so, automatically wrap to next line when appending at the end
    // We check if we're at the end of the line and the line is at screen width
    if (pos == current->size && current->size >= state->col_count - 1) {
        // Auto-wrap: insert newline and move to next line
        insert_newline(state);
        state->cursor_x = 1;
        if (state->cursor_y >= state->row_count - 1) {
            state->page_offset_y++;
            // Need to scroll the screen
            printf("\033[S");
            move_cursor(1, state->row_count - 1);
            clear_line();
        } else {
            state->cursor_y++;
        }
        // Now insert the character at the new line
        current = get_current_row(state);
        if (current != NULL && current != state->head) {
            current->size++;
            current->data = (char*)realloc(current->data, (current->size + 1) * sizeof(char));
            if (current->data == NULL) return;
            current->data[current->size - 1] = ch;
            current->data[current->size] = '\0';
            state->cursor_x = 2; // Set to 2 since we just inserted one character
            printf("%c", ch);
        }
        return;
    }
    
    if (pos == current->size) {
        // Append at end
        current->size++;
        current->data = (char*)realloc(current->data, (current->size + 1) * sizeof(char));
        if (current->data == NULL) return;
        current->data[current->size - 1] = ch;
        current->data[current->size] = '\0';
        printf("%c", ch);
        // cursor_x will be incremented by handle_char_input
    } else {
        // Insert in middle
        current->size++;
        current->data = (char*)realloc(current->data, (current->size + 1) * sizeof(char));
        if (current->data == NULL) return;
        
        for (int i = current->size - 1; i > pos; i--) {
            current->data[i] = current->data[i - 1];
        }
        current->data[pos] = ch;
        current->data[current->size] = '\0';
        
        clear_line();
        move_cursor(1, state->cursor_y);
        printf("%s", current->data);
    }
}

void insert_newline(EditorState* state) {
    Node* current = get_current_row(state);
    if (current == NULL || current == state->head) {
        // Create first two rows
        Node* row1 = create_node(NULL, 0, 1);
        Node* row2 = create_node(NULL, 0, 2);
        if (row1 == NULL || row2 == NULL) return;
        
        state->head->next = row1;
        row1->prev = state->head;
        row1->next = row2;
        row2->prev = row1;
        row2->next = NULL;
        return;
    }
    
    int pos = state->cursor_x - 1;
    Node* new_row = create_node(NULL, 0, current->row + 1);
    if (new_row == NULL) return;
    
    if (pos == current->size) {
        // Split at end - new row is empty
        new_row->next = current->next;
        if (current->next != NULL) {
            current->next->prev = new_row;
        }
        current->next = new_row;
        new_row->prev = current;
        
        // Update row numbers
        Node* tmp = new_row->next;
        while (tmp != NULL) {
            tmp->row++;
            tmp = tmp->next;
        }
    } else {
        // Split in middle
        int right_size = current->size - pos;
        char* right_data = (char*)malloc((right_size + 1) * sizeof(char));
        if (right_data == NULL) {
            free(new_row);
            return;
        }
        
        memcpy(right_data, current->data + pos, right_size);
        right_data[right_size] = '\0';
        
        current->data = (char*)realloc(current->data, (pos + 1) * sizeof(char));
        if (current->data == NULL) {
            free(right_data);
            free(new_row);
            return;
        }
        current->data[pos] = '\0';
        current->size = pos;
        
        new_row->data = right_data;
        new_row->size = right_size;
        new_row->next = current->next;
        if (current->next != NULL) {
            current->next->prev = new_row;
        }
        current->next = new_row;
        new_row->prev = current;
        
        // Update row numbers
        Node* tmp = new_row->next;
        while (tmp != NULL) {
            tmp->row++;
            tmp = tmp->next;
        }
        
        clear_line();
        move_cursor(1, state->cursor_y);
        printf("%s", current->data);
        printf("\n");
        clear_line();
        printf("%s", new_row->data);
    }
}

/* Text Deletion Functions */
void delete_char_at_cursor(EditorState* state) {
    Node* current = get_current_row(state);
    if (current == NULL || current == state->head) return;
    
    if (state->cursor_x > 1) {
        // Delete character
        int pos = state->cursor_x - 1;
        for (int i = pos - 1; i < current->size - 1; i++) {
            current->data[i] = current->data[i + 1];
        }
        current->size--;
        current->data = (char*)realloc(current->data, (current->size + 1) * sizeof(char));
        if (current->data == NULL) return;
        current->data[current->size] = '\0';
        state->cursor_x--;
        
        clear_line();
        move_cursor(1, state->cursor_y);
        printf("%s", current->data);
    } else {
        // Delete line (merge with previous)
        if (current->prev == state->head) {
            // First row - just move cursor to beginning
            state->cursor_x = 1;
            // If we're not at the top of the screen, adjust cursor_y
            if (state->cursor_y > 1) {
                state->cursor_y = 1;
            } else if (state->page_offset_y > 0) {
                state->page_offset_y--;
            }
            return;
        }
        
        Node* prev = current->prev;
        int new_size = prev->size + current->size;
        prev->data = (char*)realloc(prev->data, (new_size + 1) * sizeof(char));
        if (prev->data == NULL) return;
        
        memcpy(prev->data + prev->size, current->data, current->size);
        prev->size = new_size;
        prev->data[new_size] = '\0';
        prev->next = current->next;
        if (current->next != NULL) {
            current->next->prev = prev;
        }
        
        // Update row numbers
        Node* tmp = prev->next;
        while (tmp != NULL) {
            tmp->row--;
            tmp = tmp->next;
        }
        
        free(current->data);
        free(current);
        
        state->cursor_y--;
        state->cursor_x = prev->size + 1;
        
        clear_line();
        move_cursor(1, state->cursor_y);
        printf("%s", prev->data);
    }
}

/* Input Handling Functions */
void handle_char_input(EditorState* state, char ch) {
    int old_cursor_x = state->cursor_x;
    int old_cursor_y = state->cursor_y;
    
    insert_char_at_position(state, ch);
    
    // Only increment cursor_x if auto-wrap didn't occur
    // (auto-wrap sets cursor_x to 2 internally)
    if (state->cursor_x == old_cursor_x && state->cursor_y == old_cursor_y) {
        state->cursor_x++;
    }
    
    // Update display based on cursor position
    if (state->cursor_y == state->row_count - 1) {
        update_status_bar(state);
        move_cursor(state->cursor_x, state->row_count - 1);
    } else {
        Node* last_row = state->head;
        while (last_row != NULL && last_row->next != NULL) {
            last_row = last_row->next;
        }
        if (last_row != state->head && last_row->row > state->row_count - 1) {
            update_status_bar(state);
        }
        move_cursor(state->cursor_x, state->cursor_y);
    }
}

void handle_backspace(EditorState* state) {
    delete_char_at_cursor(state);
    move_cursor(state->cursor_x, state->cursor_y);
}

void handle_enter(EditorState* state) {
    Node* last_row = state->head;
    while (last_row != NULL && last_row->next != NULL) {
        last_row = last_row->next;
    }
    
    if (state->cursor_y == state->row_count - 1) {
        printf("\033[S");
        move_cursor(1, state->row_count - 1);
        clear_line();
        move_cursor(1, state->row_count - 1);
        insert_newline(state);
        state->cursor_x = 1;
        state->page_offset_y++;
        update_status_bar(state);
        move_cursor(state->cursor_x, state->row_count - 1);
    } else if (last_row != state->head && last_row->row > state->row_count - 2) {
        insert_newline(state);
        state->cursor_x = 1;
        state->cursor_y++;
        move_cursor(state->cursor_x, state->cursor_y);
    } else {
        insert_newline(state);
        state->cursor_x = 1;
        state->cursor_y++;
        move_cursor(state->cursor_x, state->cursor_y);
    }
}

void handle_arrow_keys(EditorState* state, int key) {
    Node* current = get_current_row(state);
    if (current == NULL || current == state->head) return;
    
    switch (key) {
        case UP:
            if (state->cursor_y == 1 && current->prev != state->head) {
                state->page_offset_y--;
                printf("\033[T");
                clear_line();
                move_cursor(1, 1);
                Node* print_row = get_current_row(state);
                if (print_row != NULL && print_row != state->head) {
                    printf("%s", print_row->data != NULL ? print_row->data : "");
                }
                if (current->prev != NULL && current->prev != state->head) {
                    if (current->prev->size < state->cursor_x - 1) {
                        state->cursor_x = current->prev->size + 1;
                    }
                    move_cursor(state->cursor_x, 1);
                }
            } else {
                if (current->prev != NULL && current->prev != state->head && state->cursor_y > 1) {
                    if (current->prev->size < state->cursor_x - 1) {
                        state->cursor_x = current->prev->size + 1;
                    }
                    state->cursor_y--;
                    move_cursor(state->cursor_x, state->cursor_y);
                }
            }
            break;
            
        case DOWN:
            if (state->cursor_y >= state->row_count - 1 && current->next != NULL) {
                state->page_offset_y++;
                printf("\033[S");
                clear_line();
                move_cursor(1, state->cursor_y);
                Node* print_row = get_current_row(state);
                if (print_row != NULL && print_row != state->head) {
                    printf("%s", print_row->data != NULL ? print_row->data : "");
                }
                if (current->next != NULL) {
                    if (current->next->size < state->cursor_x - 1) {
                        state->cursor_x = current->next->size + 1;
                    }
                    move_cursor(state->cursor_x, state->cursor_y);
                }
            } else {
                if (current->next != NULL) {
                    if (current->next->size < state->cursor_x - 1) {
                        state->cursor_x = current->next->size + 1;
                    }
                    state->cursor_y++;
                    move_cursor(state->cursor_x, state->cursor_y);
                }
            }
            break;
            
        case LEFT:
            if (current->size >= 1 && state->cursor_x > 1) {
                state->cursor_x--;
                move_cursor(state->cursor_x, state->cursor_y);
            } else if (state->cursor_x == 1) {
                if (current->prev != state->head) {
                    state->cursor_x = current->prev->size + 1;
                    state->cursor_y--;
                    move_cursor(state->cursor_x, state->cursor_y);
                } else {
                    state->cursor_x = 1;
                    state->cursor_y = 1;
                    move_cursor(state->cursor_x, state->cursor_y);
                }
            }
            break;
            
        case RIGHT:
            if (current->size >= state->cursor_x) {
                state->cursor_x++;
                move_cursor(state->cursor_x, state->cursor_y);
            } else if (current->size == state->cursor_x - 1 && current->next == NULL) {
                move_cursor(state->cursor_x, state->cursor_y);
            } else {
                state->cursor_x = 1;
                state->cursor_y++;
                move_cursor(state->cursor_x, state->cursor_y);
            }
            break;
            
        case HOME:
            state->cursor_x = 1;
            move_cursor(state->cursor_x, state->cursor_y);
            break;
            
        case END:
            state->cursor_x = current->size + 1;
            move_cursor(state->cursor_x, state->cursor_y);
            break;
            
        case PAGEUP:
            {
                // Move up by one screen height
                int page_size = state->row_count - 2; // Exclude status bar and help bar
                int moved = 0;
                for (int i = 0; i < page_size; i++) {
                    if (state->cursor_y == 1 && current->prev != state->head) {
                        state->page_offset_y--;
                        current = get_current_row(state);
                        moved++;
                    } else if (current->prev != NULL && current->prev != state->head && state->cursor_y > 1) {
                        if (current->prev->size < state->cursor_x - 1) {
                            state->cursor_x = current->prev->size + 1;
                        }
                        state->cursor_y--;
                        current = current->prev;
                        moved++;
                    } else {
                        break; // Can't move up further
                    }
                }
                if (moved > 0) {
                    refresh_screen(state);
                    if (current != NULL && current != state->head) {
                        if (current->size < state->cursor_x - 1) {
                            state->cursor_x = current->size + 1;
                        }
                        move_cursor(state->cursor_x, state->cursor_y);
                    }
                }
            }
            break;
            
        case PAGEDOWN:
            {
                // Move down by one screen height
                int page_size = state->row_count - 2; // Exclude status bar and help bar
                int moved = 0;
                for (int i = 0; i < page_size; i++) {
                    if (state->cursor_y >= state->row_count - 1 && current->next != NULL) {
                        state->page_offset_y++;
                        current = get_current_row(state);
                        moved++;
                    } else if (current->next != NULL) {
                        if (current->next->size < state->cursor_x - 1) {
                            state->cursor_x = current->next->size + 1;
                        }
                        state->cursor_y++;
                        current = current->next;
                        moved++;
                    } else {
                        break; // Can't move down further
                    }
                }
                if (moved > 0) {
                    refresh_screen(state);
                    if (current != NULL && current != state->head) {
                        if (current->size < state->cursor_x - 1) {
                            state->cursor_x = current->size + 1;
                        }
                        move_cursor(state->cursor_x, state->cursor_y);
                    }
                }
            }
            break;
    }
}

/* Editor State Management */
void init_editor_state(EditorState* state) {
    memset(state, 0, sizeof(EditorState));
    state->cursor_x = 1;
    state->cursor_y = 1;
    state->page_offset_y = 0;
    state->has_filename = FALSE;
    init_node_list(state);
}

void free_editor_state(EditorState* state) {
    Node* current = state->head;
    while (current != NULL) {
        Node* next = current->next;
        if (current->data != NULL) {
            free(current->data);
        }
        free(current);
        current = next;
    }
}
