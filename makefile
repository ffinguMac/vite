CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = vite
SOURCES = vite.c terminal.c editor.c display.c file_io.c
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo Cleaning up object files...
ifeq ($(OS),Windows_NT)
	@if exist vite.o del vite.o
	@if exist terminal.o del terminal.o
	@if exist editor.o del editor.o
	@if exist display.o del display.o
	@if exist file_io.o del file_io.o
else
	rm -f $(OBJECTS)
endif

%.o: %.c vite.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	@if exist vite.o del vite.o
	@if exist terminal.o del terminal.o
	@if exist editor.o del editor.o
	@if exist display.o del display.o
	@if exist file_io.o del file_io.o
	@if exist vite.exe del vite.exe
else
	rm -f $(OBJECTS) $(TARGET)
endif

.PHONY: clean
