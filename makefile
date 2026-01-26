CC = gcc
CFLAGS = -Wall -Wextra -Werror
SOURCES = vite.c terminal.c editor.c display.c file_io.c
OBJECTS = $(SOURCES:.c=.o)

# OS-specific settings: Windows uses .exe, Unix (macOS/Linux) uses no extension
ifeq ($(OS),Windows_NT)
    TARGET = vite.exe
else
    TARGET = vite
endif

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo Cleaning up object files...
ifeq ($(OS),Windows_NT)
	@if exist vite.o del /Q vite.o 2>nul
	@if exist terminal.o del /Q terminal.o 2>nul
	@if exist editor.o del /Q editor.o 2>nul
	@if exist display.o del /Q display.o 2>nul
	@if exist file_io.o del /Q file_io.o 2>nul
else
	rm -f $(OBJECTS)
endif

%.o: %.c vite.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	@if exist vite.o del /Q vite.o 2>nul
	@if exist terminal.o del /Q terminal.o 2>nul
	@if exist editor.o del /Q editor.o 2>nul
	@if exist display.o del /Q display.o 2>nul
	@if exist file_io.o del /Q file_io.o 2>nul
	@if exist $(TARGET) del /Q $(TARGET) 2>nul
else
	rm -f $(OBJECTS) $(TARGET)
endif

.PHONY: clean
