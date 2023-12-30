#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

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
#define ctrls 19
#define ctrlq 17
#define ctrlf 6
#define clear "cls"
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
#define ctrls 19
#define ctrlq 17
#define ctrlf 6
#define clear "clear"
#endif

typedef struct Node {
    char* data;
    int row;
    int size;
    struct Node* next;
    struct Node* prev;
} Node;

Node* head = NULL;

int x = 1;
int y = 1;
int p_y = 1;

void goCursor(int xx, int yy) {
  printf("\033[%d;%dH", yy, xx); // 커서를 움직이는 함수
}

void delete(){
  printf("\033[2K"); // 그 줄을 지우는 함수, 커서는 위치가 유지된다.
}

void printE(int row) {
  Node* currentRow = head;
  for (int i = 0; i < p_y + y + 1 ; i++) //커서 위치까지 이동
    currentRow = currentRow->next;
  while (currentRow->next != NULL && currentRow->row - p_y <= row - 1) { // 다음 줄 부터 출력
    delete();
    printf("%s\n", currentRow->data);
    currentRow = currentRow->next;
  }
  printf("%s\n", currentRow->data);
}

void printAll(int line, int row) {
  Node* currentRow = head;
  for (int i = 0; i < p_y + line; i++)
    currentRow = currentRow->next;
  while (currentRow->next != NULL && currentRow->row <= p_y + row - 1) {
    printf("%s\n", currentRow->data);
    currentRow = currentRow->next;
  }
  printf("%s", currentRow->data);
}

void initStruct() {
  head = (Node*)malloc(sizeof(Node));
  head->next = NULL;
  head->prev = NULL;
  head->data = NULL;
  head->row = 0;
  head->size = 0;
}

void insertChar(int data) {
  if (head->next == NULL) { // 첫 입력
    Node* newRow = (Node*)malloc(sizeof(Node));
    newRow->size = 1;
    newRow->data = (char*)malloc(newRow->size + 1 * sizeof(char));
    newRow->data[0] = data;
    newRow->data[1] = '\0';
    newRow->next = NULL;
    head->next = newRow;
    newRow->prev = head;
    newRow->row = 1;
    printf("%c", data);
  }
  else{
    Node* currentRow = head;
    for (int i = 0; i < y + p_y - 1; i++)
      currentRow = currentRow->next;
    if(currentRow->size == x - 1) { // 문자열 마지막 입력
      currentRow->size++;
      currentRow->data = (char *) realloc(currentRow->data, currentRow->size + 1 * sizeof(char));
      currentRow->data[currentRow->size - 1] = data;
      currentRow->data[currentRow->size] = '\0';
      printf("%c", data);
    }
    else{ // 중간 입력
      currentRow->size++;
      currentRow->data = (char *) realloc(currentRow->data, currentRow->size + 1 * sizeof(char));
      for (int i = currentRow->size - 1; i > x - 1 ; i--){
        currentRow->data[i] = currentRow->data[i-1];
      }
      currentRow->data[x - 1] = data;
      currentRow->data[currentRow->size] = '\0';
      delete();
      goCursor(1, y);
      printf("%s", currentRow->data);
    }
  }
}

void insertEnter(int row) {
  Node* newRow = (Node*)malloc(sizeof(Node)); // 새 행 생성
  newRow->size = 0;
  newRow->data = (char*)malloc(newRow->size + 1 * sizeof(char));
  newRow->data[0] = '\0';
  newRow->next = NULL;
  newRow->prev = NULL;
  Node* currentRow = head;
  if (currentRow -> next == NULL){
    Node* nextRow = (Node*)malloc(sizeof(Node));
    nextRow->size = 0;
    nextRow->data = (char*)malloc(newRow->size + 1 * sizeof(char));
    nextRow->data[0] = '\0';
    head->next = newRow;
    newRow->next = nextRow;
    newRow->prev = head;
    nextRow->next = NULL;
    nextRow->prev = newRow;
    newRow->row = 1;
    nextRow->row = 2;
    return;
  }
  for (int i = 0; i < y + p_y - 1; i++)
    currentRow = currentRow->next;
  if (currentRow->next == NULL) { // 다음 행이 없을때
    if (currentRow->size == x - 1) { // 젤 끝 삽입
      newRow->row = currentRow->row + 1;
      currentRow->next = newRow;
      newRow->prev = currentRow;
      newRow->next = NULL;
      newRow->data = (char *) malloc(newRow->size + 1 * sizeof(char));
      newRow->data[0] = '\0';
    }
    else { // 중간 삽입
      newRow->row = currentRow->row + 1;
      currentRow->next = newRow;
      newRow->prev = currentRow;
      newRow->next = NULL;
      char *nexttmp = (char *) malloc(currentRow->size - x + 1 * sizeof(char));
      char *currtmp = (char *) malloc(x-1 * sizeof(char));
      strncpy(nexttmp, currentRow->data + x - 1, currentRow->size - x + 1);
      strncpy(currtmp, currentRow->data, x);
      nexttmp[currentRow->size - x + 1] = '\0';
      currtmp[x-1] = '\0';
      newRow->data = nexttmp;
      currentRow->data = currtmp;
      newRow->size = currentRow->size - x + 1;
      currentRow->size = x-1;
      goCursor(0, y);
      delete();
      printf("%s", currentRow->data);
      printf("\n%s", newRow->data);
    }
  } else{
    if (currentRow->size == x - 1) { // 다음 행이 있고 젤 끝
      currentRow = head;
      for (int i = 0; i < y + p_y - 1; i++)
        currentRow = currentRow->next;
      Node* tmpRow = currentRow->next;
      newRow->row = currentRow->row + 1;
      newRow->prev = currentRow;
      newRow->next = tmpRow;
      tmpRow->prev = newRow;
      currentRow->next = newRow;
      tmpRow->row = newRow->row+1;

      newRow->data = (char*)malloc(newRow->size + 1 * sizeof(char));
      newRow->data[0] = '\0';
      goCursor(1, y+1);
      delete();
      printf("%s\n", newRow->data);
      while (tmpRow->next != NULL){
        tmpRow = tmpRow->next;
        tmpRow->row++;
      }
      printE(row);

    }
    else { // 다음 행이 있고 중간 삽입
      currentRow = head;
      for (int i = 0; i < y + p_y - 1; i++)
        currentRow = currentRow->next;
      Node* tmpRow = currentRow->next;
      currentRow->next = newRow;
      newRow->prev = currentRow;
      newRow->next = tmpRow;
      tmpRow->prev = newRow;
      char *nexttmp = (char *) malloc(currentRow->size - x + 1 * sizeof(char));
      char *currtmp = (char *) malloc(x-1 * sizeof(char));
      strncpy(nexttmp, currentRow->data + x - 1, currentRow->size - x + 1);
      strncpy(currtmp, currentRow->data, x);
      nexttmp[currentRow->size - x + 1] = '\0';
      currtmp[x-1] = '\0';
      newRow->data = nexttmp;
      currentRow->data = currtmp;
      newRow->size = currentRow->size - x + 1;
      currentRow->size = x-1;
      newRow->row = currentRow->row + 1;
      tmpRow->row = newRow->row + 1;
      goCursor(1, y);
      delete();
      printf("%s\n", currentRow->data);
      delete();
      printf("%s\n", newRow->data);
      while (tmpRow->next != NULL){
        tmpRow = tmpRow->next;
        tmpRow->row++;
      }
      printE(row);
    }
  }
}

void deleteChar(int row) {
  Node* currentRow = head;
  for (int i = 0; i < p_y + y - 1; i++)
    currentRow = currentRow->next;
  if (x != 1) { // 문자를 삭제하는 경우
    x--;
    for (int i = x - 1; i < currentRow->size; i++) {
      currentRow->data[i] = currentRow->data[i + 1];
    }
    currentRow->size--;
    currentRow->data = (char *) realloc(currentRow->data, currentRow->size + 1 * sizeof(char));
    currentRow->data[currentRow->size] = '\0';
    goCursor(1, y);
    delete();
    printf("%s", currentRow->data);
  }
  else { //줄을 삭제하는 경우
    y--;
    if(currentRow-> prev != head && currentRow->next == NULL){ //첫 행이 아니고, 다음 행이 없을 때
      delete();
      printf("~");
      currentRow->prev->data = (char *) realloc(currentRow->prev->data, (currentRow->size + currentRow->prev->size + 1) * sizeof(char));
      int j = 0;
      for (int i = currentRow->prev->size; i < currentRow->prev->size + currentRow->size; i++)
        currentRow->prev->data[i] = currentRow->data[j++];
      currentRow->prev->size += currentRow->size;
      currentRow->prev->data[currentRow->prev->size] = '\0';
      currentRow->prev->next = NULL;
      goCursor(1, y);
      delete();
      printf("%s", currentRow->prev->data);
      x = currentRow->prev->size - currentRow->size + 1;
      goCursor(x, y);
    }
    else if (currentRow-> prev != head && currentRow->next != NULL) { // 첫 행이 아니고, 다음 행이 있을 때
      delete();
      Node* tmpRow = currentRow->prev;
      int newsize = tmpRow->size + currentRow->size;
      x = tmpRow->size + 1;
      tmpRow->data = (char *) realloc(tmpRow->data,
                                      (currentRow->size + tmpRow->size + 1) * sizeof(char));
      int j = 0;
      for (int i = tmpRow->size; i < tmpRow->size + currentRow->size; i++)
        tmpRow->data[i] = currentRow->data[j++];
      tmpRow->size += currentRow->size;
      tmpRow->data[newsize] = '\0';
      tmpRow->next = currentRow->next;
      currentRow->next->prev = tmpRow;
      goCursor(0, y);
      printf("%s", tmpRow->data);
      while (tmpRow->next != NULL) {
        printf("\n");
        delete();
        tmpRow->next->row--;
        tmpRow = tmpRow->next;
      }
      goCursor(0, y + 1);
      printf("%s\n", currentRow->next->data);
      printE(row);
      printf("\n");
      printf("~");
      goCursor(x, y);
    } else { // 첫 행 일 때
      x = 1, y = 1;
      goCursor(x, y);
    }
  }
}

void setVite(int row, int col) {
  int n = 29;
  for (int i = 0; i < row; i++)
    printf("~\n");
  printf("\033[7m[No Name] - 0 lines");
  for (int i = 0; i < col - n; i++)
    printf(" ");
  printf("no ft | %d/%d", x-1, y-1);
  printf("\n\033[27m");
  printf("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
  printf("\033[H");
}

int cnt(int num) {
  if (num == 0)
    return 1;
  int cnt = 0;
  while (num > 0) {
    num /= 10;
    cnt++;
  }
  return cnt;
}

void setState(int row, int col, int line) { // argc가 1일 때
  int l, xx, yy;
  l = cnt(line);
  xx = cnt(y + p_y - 1);
  yy = cnt(line - 1);
  int n = 26 + l + xx + yy;
  goCursor(0, row);
  delete();
  printf("\033[7m[No Name] - %d lines", line);
  for (int i = 0; i < col - n ; i++)
    printf(" ");
  printf("no ft | %d/%d", y + p_y - 1, line);
  printf("\n\033[27m");
  delete();
}

void setReadState(int row, int col, int line, char*filename) { // argc가 2일 때
  int l, xx, yy, fname;
  l = cnt(line);
  xx = cnt(y + p_y - 1);
  yy = cnt(line - 1);
  fname = strlen(filename);
  int n = 19 + l + xx + yy + fname;
  goCursor(0, row);
  delete();
  printf("\033[7m[%s] - %d lines", filename, line);
  for (int i = 0; i < col - n; i++)
    printf(" ");
  printf("no ft | %d/%d", y + p_y - 1, line);
  printf("\n\033[27m");
  delete();
}

void save(char* filename) {
  FILE* file = fopen(filename, "wt");

  Node* currentRow = head;
  while (currentRow->next != NULL) {
    currentRow = currentRow->next;
    fputs(currentRow->data, file);
    if (currentRow->next != NULL)
      fputc('\n', file);
  }

  fclose(file);
}

void saveReadFile(char* filename) {
  FILE* file = fopen(filename, "wt");

  Node* currentRow = head;
  while (currentRow->next != NULL) {
    currentRow = currentRow->next;
    fputs(currentRow->data, file);
    if (currentRow->next != NULL) // 마지막 개행 제외
      fputc('\n', file);
  }

  fclose(file);
}

int getch() {
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

void readFile(char* filename, int* error) {
  FILE* file = fopen(filename, "rt");
  if (file == NULL) {
    *error = TRUE;
    return;
  }

  char str[100000];
  int row = 1;

  while (fgets(str, sizeof(str), file) != NULL) {
    int n = strlen(str);

    if (n > 0 && (str[n - 1] == '\n' || str[n - 1] == '\r')) { // 개행 제거
      str[--n] = '\0';
    }

    Node* newRow = (Node*)malloc(sizeof(Node));

    newRow->size = n;
    newRow->data = (char*)malloc(newRow->size + 1);


    strcpy(newRow->data, str);
    newRow->data[newRow->size] = '\0';
    newRow->next = NULL;

    if (head == NULL) {
      head = newRow;
      newRow->prev = NULL;
    } else {
      Node* currentRow = head;
      while (currentRow->next != NULL)
        currentRow = currentRow->next;

      currentRow->next = newRow;
      newRow->prev = currentRow;
    }

    newRow->row = row;
    row++;
  }
  y = row;
  fclose(file);
}

void print(int row) { //읽어온 파일을 출력하는 함수
  Node* currentRow = head;
  while (currentRow->next != NULL && currentRow->row <= row - 2) {
    currentRow = currentRow->next;
    printf("%s\n", currentRow->data);
  }
}

int DisableWrap() { //wrapping을 끄게 해주는 함수
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

void highLight(char *text, char *pattern) {
  char *found = strstr(text, pattern);

  if (found != NULL) {
    fwrite(text, 1, found - text, stdout);

    printf("\033[7m%s\033[27m", pattern);

    highLight(found + strlen(pattern), pattern);
  } else {
    printf("%s", text);
  }
}

int main(int argc, char** argv) {
  int row, col;
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
  row = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top;
  col = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left;
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
  {
    printf("Couldn't get the console handle. Quitting.\n");
    return -1;
  }
#else
  struct winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
  row = size.ws_row - 1;
  col = size.ws_col - 1;
#endif
  char filename[100];
  int exitFlag = FALSE;
  int nameFlag = FALSE;
  initStruct();
  system(clear);
  DisableWrap();
  if (argc == 1) {
    setVite(row, col);
    while (1) {
      int ch;
      ch = getch();
      Node* currentRow = head;
      while (currentRow->next != NULL)
        currentRow = currentRow->next;
      if (32 <= ch && ch <= 126) {
        if (y == row - 1) {
          insertChar(ch);
          x++;
          if (nameFlag)
            setReadState(row, col, currentRow->row, filename);
          else
            setState(row, col, currentRow->row);
          goCursor(x, row-1);
          continue;
        } else if (currentRow->row > row - 1) {
          insertChar(ch);
          x++;
          if (nameFlag)
            setReadState(row, col, currentRow->row, filename);
          else
            setState(row, col, currentRow->row);
          goCursor(x, y);
          continue;
        } else {
          insertChar(ch);
          goCursor(++x, y);
        }
      } else if (ch == BACKSPACE) {
        deleteChar(row);
        goCursor(x, y);
      } else if (ch == ENTER) {
        if (y == row-1) {
          printf("\033[S");
          goCursor(1, row-1);
          delete();
          goCursor(1, row-1);
          insertEnter(row);
          x = 1;
          p_y++;
          if (nameFlag)
            setReadState(row, col, currentRow->row + 1, filename);
          else
            setState(row, col, currentRow->row + 1);
          goCursor(x, row-1);
          continue;
        }else if (currentRow->row > row - 2) {
          insertEnter(row);
          x = 1;
          goCursor(x, ++y);
        }else {
          insertEnter(row);
          x = 1;
          goCursor(x, ++y);
        }
      } else if (ch == ctrls) {
        if (nameFlag) {
          goCursor(1, row + 1);
          delete();
          saveReadFile(filename);
          system(clear);
          printAll(0, row);
          save(filename);
          goCursor(1, row);
          setReadState(row, col, currentRow->row, filename);
          goCursor(1, row+1);
          printf("file save!");
          goCursor(x, y);
          continue;
        } else {
          goCursor(1, row + 1);
          delete();
          printf("file name : ");
          scanf("%s", filename);
          system(clear);
          printAll(0, row);
          save(filename);
          goCursor(1, row);
          setReadState(row, col, currentRow->row, filename);
          goCursor(1, row + 1);
          printf("file save!");
          goCursor(x, y);
          nameFlag = TRUE;
          continue;
        }
      } else if (ch == ctrlq) {
        if (exitFlag == TRUE) {
          goCursor(1, row + 1);
          delete();
          printf("exit!!");
          return 0;
        }
        exitFlag = TRUE;
        continue;
      } else if (ch == ctrlf) {
        char word[100];
        goCursor(1, row + 1);
        delete();
        printf("find word : ");
        scanf("%s", word);
        system(clear);
        Node* currentRow = head;
        while (currentRow->next != NULL && currentRow->row <= row -2) {
          currentRow = currentRow->next;
          highLight(currentRow->data, word);
          printf("\n");
        }
        if (nameFlag)
          setReadState(row, col, currentRow->row, filename);
        else
          setState(row, col, currentRow->row);
        goCursor(x, y);
        continue;
      }
#ifdef _WIN32
      else if (ch == 224 || ch == 0) {
        currentRow = head;
        for (int i = 0; i < y + p_y - 1; i++)
          currentRow = currentRow->next;
        ch = getch();
        if (currentRow == NULL)
          continue;
        switch (ch) {
          case UP:
            if (y == 1 && currentRow->prev != head) {
              p_y--;
              printf("\033[T");
              delete();
              goCursor(1, 1);
              Node* printRow = head;
              for (int i = 0; i < p_y + y - 1; i++)
                printRow = printRow->next;
              printf("%s", printRow->data);
              if (currentRow->prev != NULL && y != 1) {
                if (currentRow->prev->size <= x - 1) {
                  x = currentRow->prev->size + 1;
                  goCursor(x, 1);
                } else {
                  goCursor(x, 1);
                }
              }
            } else {
              if (currentRow->prev != NULL && y != 1) {
                if (currentRow->prev->size <= x - 1) {
                  x = currentRow->prev->size + 1;
                  goCursor(x, --y);
                } else {
                  goCursor(x, --y);
                }
              }
            }
            break;
          case DOWN:
            if (y >= row - 1 && currentRow->next != NULL) {
              p_y++;
              printf("\033[S");
              delete();
              goCursor(1, y);
              Node* printRow = head;
              for (int i = 0; i < p_y + y - 1; i++)
                printRow = printRow->next;
              printf("%s", printRow->data);
              if (currentRow->next != NULL) {
                if (currentRow->next->size <= x - 1) {
                  x = currentRow->next->size + 1;
                  goCursor(x, y);
                } else
                  goCursor(x, y);
              }
            } else {
              if (currentRow->next != NULL) {
                if (currentRow->next->size <= x - 1) {
                  x = currentRow->next->size + 1;
                  goCursor(x, ++y);
                } else
                  goCursor(x, ++y);
              }
            }
            break;
          case LEFT:
            if (currentRow->size > 1 && x > 1)
              goCursor(--x, y);
            else if (x == 1){
              if (currentRow->prev != head){
                x = currentRow->prev->size + 1;
                goCursor(x, --y);
              } else {
                y = 1;
                x = 1;
                goCursor(x, y);
              }
            }
            break;
          case RIGHT:
            if (currentRow->size >= x)
              goCursor(++x, y);
            else if (currentRow->size == x - 1 && currentRow->next == NULL){
              goCursor(x, y);
            }
            else {
              x = 1;
              goCursor(x, ++y);
            }
            break;
          case HOME:
            x = 1;
            goCursor(x, y);
            break;
          case END:
            x = currentRow->size + 1;
            goCursor(x, y);
            break;
          case PAGEUP:
            break;
          case PAGEDOWN:
            break;
        }
      }
#else
      else if (ch == 27) {
        Node *currentRow = head;
        for (int i = 0; i < p_y + y - 1; i++)
          currentRow = currentRow->next;
        ch = getch();
        if (ch == '[') {
          ch = getch();
          if (currentRow == NULL)
          continue;
          switch (ch) {
            case UP: // 위쪽 방향키
              if (y == 1 && currentRow->prev != head) {
                p_y--;
                printf("\033[T");
                delete();
                goCursor(1, 1);
                Node* printRow = head;
                for (int i = 0; i < p_y + y - 1; i++)
                  printRow = printRow->next;
                printf("%s", printRow->data);
                if (currentRow->prev != NULL && y != 1) {
                  if (currentRow->prev->size <= x - 1) {
                    x = currentRow->prev->size + 1;
                    goCursor(x, 1);
                  } else {
                    goCursor(x, 1);
                  }
                }
              } else {
                if (currentRow->prev != NULL && y != 1) {
                  if (currentRow->prev->size <= x - 1) {
                    x = currentRow->prev->size + 1;
                    goCursor(x, --y);
                  } else {
                    goCursor(x, --y);
                  }
                }
              }
              break;
            case DOWN: // 아래쪽 방향키
              if (y >= row - 1 && currentRow->next != NULL) {
                p_y++;
                printf("\033[S");
                delete();
                goCursor(1, y);
                Node* printRow = head;
                for (int i = 0; i < p_y + y - 1; i++)
                  printRow = printRow->next;
                printf("%s", printRow->data);
                if (currentRow->next != NULL) {
                  if (currentRow->next->size <= x - 1) {
                    x = currentRow->next->size + 1;
                    goCursor(x, y);
                  } else
                    goCursor(x, y);
                }
              } else {
                if (currentRow->next != NULL) {
                  if (currentRow->next->size <= x - 1) {
                    x = currentRow->next->size + 1;
                    goCursor(x, ++y);
                  } else
                    goCursor(x, ++y);
                }
              }
              break;
            case LEFT: // 왼쪽 방향키
              if (currentRow->size > 1 && x > 1)
                goCursor(--x, y);
              else if (x == 1){
                if (currentRow->prev != head){
                  x = currentRow->prev->size + 1;
                  goCursor(x, --y);
                } else {
                  y = 1;
                  x = 1;
                  goCursor(x, y);
                }
              }
              break;
            case RIGHT: // 오른쪽 방향키
              if (currentRow->size >= x)
                goCursor(++x, y);
              else if (currentRow->size == x - 1 && currentRow->next == NULL){
                goCursor(x, y);
              }
              else {
                x = 1;
                goCursor(x, ++y);
              }
              break;
            case PAGEUP:
              getch();
              break;
            case PAGEDOWN:
              getch();
              break;
          }
        } else if (ch == '0'){
          ch = getch();
          switch (ch)
          {
          case HOME:
            x = 1;
            goCursor(x, y);
            break;
          case END:
            x = currentRow->size + 1;
            goCursor(x, y);
            break;
          }
        }
      }
#endif
      currentRow = head;
      while (currentRow-> next != NULL)
        currentRow = currentRow->next;
      if (nameFlag)
        setReadState(row, col, currentRow->row, filename);
      else
        setState(row, col, currentRow->row);
      goCursor(x, y);
      exitFlag = FALSE;
    }
  }else if (argc == 2) {
    int error = FALSE;
    readFile(argv[1], &error);
    if (error)
      return 0;
    setVite(row, col);
    print(row);
    y--;
    setReadState(row, col, y, argv[1]);
    goCursor(1, row + 1);
    printf("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
    y = 1;
    x = 1;
    goCursor(1, 1);
    while (1) {
      int ch;
      ch = getch();
      Node* currentRow = head;
      while (currentRow->next != NULL)
        currentRow = currentRow->next;
      if (32 <= ch && ch <= 126) {
        if (y == row - 1) {
          insertChar(ch);
          x++;
          if (nameFlag)
            setReadState(row, col, currentRow->row, filename);
          else
            setState(row, col, currentRow->row);
          goCursor(x, row-1);
          continue;
        } else if (currentRow->row > row - 1) {
          insertChar(ch);
          x++;
          if (nameFlag)
            setReadState(row, col, currentRow->row, filename);
          else
            setState(row, col, currentRow->row);
          goCursor(x, y);
          continue;
        } else {
          insertChar(ch);
          goCursor(++x, y);
        }
      } else if (ch == BACKSPACE) {
        deleteChar(row);
        goCursor(x, y);
      } else if (ch == ENTER) {
        if (y == row-1) {
          printf("\033[S");
          goCursor(1, row-1);
          delete();
          goCursor(1, row-1);
          insertEnter(row);
          x = 1;
          p_y++;
          if (nameFlag)
            setReadState(row, col, currentRow->row, filename);
          else
            setState(row, col, currentRow->row);
          goCursor(x, row-1);
          continue;
        }else if (currentRow->row > row - 2) {
          insertEnter(row);
          x = 1;
          goCursor(x, ++y);
        }else {
          insertEnter(row);
          x = 1;
          goCursor(x, ++y);
        }
      } else if (ch == ctrls) {
        goCursor(1, row + 1);
        delete();
        saveReadFile(argv[1]);
        x = 1, y = 1;
        goCursor(x, y);
        system(clear);
        printAll(0, row);
        save(filename);
        goCursor(1, row);
        setReadState(row, col, currentRow->row, argv[1]);
        goCursor(1, row+1);
        printf("file save!");
        continue;
      } else if (ch == ctrlq) {
        if (exitFlag == TRUE) {
          goCursor(1, row + 1);
          printf("exit!!");
          return 0;
        }
        exitFlag = TRUE;
        continue;
      } else if (ch == ctrlf) {
        char word[100];
        goCursor(1, row + 1);
        delete();
        printf("find word : ");
        scanf("%s", word);
        system(clear);
        Node* currentRow = head;
        while (currentRow->next != NULL && currentRow->row <= row -2) {
          currentRow = currentRow->next;
          highLight(currentRow->data, word);
          printf("\n");
        }
        setReadState(row, col, currentRow->row, argv[1]);
        goCursor(x, y);
        continue;
      }
#ifdef _WIN32
      else if (ch == 224 || ch == 0) {
        currentRow = head;
        for (int i = 0; i < y + p_y - 1; i++)
          currentRow = currentRow->next;
        ch = getch();
        if (currentRow == NULL)
          continue;
        switch (ch) {
          case UP: // 위쪽 방향키
            if (y == 1 && currentRow->prev != head) {
              p_y--;
              printf("\033[T");
              delete();
              goCursor(1, 1);
              Node* printRow = head;
              for (int i = 0; i < p_y + y - 1; i++)
                printRow = printRow->next;
              printf("%s", printRow->data);
              if (currentRow->prev != NULL && y != 1) {
                if (currentRow->prev->size <= x - 1) {
                  x = currentRow->prev->size + 1;
                  goCursor(x, 1);
                } else {
                  goCursor(x, 1);
                }
              }
            } else {
              if (currentRow->prev != NULL && y != 1) {
                if (currentRow->prev->size <= x - 1) {
                  x = currentRow->prev->size + 1;
                  goCursor(x, --y);
                } else {
                  goCursor(x, --y);
                }
              }
            }
            break;
          case DOWN: // 아래쪽 방향키
            if (y >= row - 1 && currentRow->next != NULL) {
              p_y++;
              printf("\033[S");
              delete();
              goCursor(1, y);
              Node* printRow = head;
              for (int i = 0; i < p_y + y - 1; i++)
                printRow = printRow->next;
              printf("%s", printRow->data);
              if (currentRow->next != NULL) {
                if (currentRow->next->size <= x - 1) {
                  x = currentRow->next->size + 1;
                  goCursor(x, y);
                } else
                  goCursor(x, y);
              }
            } else {
              if (currentRow->next != NULL) {
                if (currentRow->next->size <= x - 1) {
                  x = currentRow->next->size + 1;
                  goCursor(x, ++y);
                } else
                  goCursor(x, ++y);
              }
            }
            break;
          case LEFT: // 왼쪽 방향키
            if (currentRow->size > 1 && x > 1)
              goCursor(--x, y);
            else if (x == 1){
              if (currentRow->prev != head){
                x = currentRow->prev->size + 1;
                goCursor(x, --y);
              } else {
                y = 1;
                x = 1;
                goCursor(x, y);
              }
            }
            break;
          case RIGHT: // 오른쪽 방향키
            if (currentRow->size >= x)
              goCursor(++x, y);
            else if (currentRow->size == x - 1 && currentRow->next == NULL){
              goCursor(x, y);
            }
            else {
              x = 1;
              goCursor(x, ++y);
            }
            break;
          case HOME:
            x = 1;
            goCursor(x, y);
            break;
          case END:
            x = currentRow->size + 1;
            goCursor(x, y);
            break;
          case PAGEUP:
            getch();
            break;
          case PAGEDOWN:
            getch();
            break;
        }
      }
#else
      else if (ch == 27) { // Arrow keys
        Node *currentRow = head;
        for (int i = 0; i < p_y + y - 1; i++)
          currentRow = currentRow->next;
        ch = getch();
        if (ch == '[') {
          ch = getch();
          if (currentRow == NULL)
            continue;
          switch (ch) {
            case UP: // 위쪽 방향키
              if (y == 1 && currentRow->prev != head) {
                p_y--;
                printf("\033[T");
                delete();
                goCursor(1, 1);
                Node* printRow = head;
                for (int i = 0; i < p_y + y - 1; i++)
                  printRow = printRow->next;
                printf("%s", printRow->data);
                if (currentRow->prev != NULL && y != 1) {
                  if (currentRow->prev->size <= x - 1) {
                    x = currentRow->prev->size + 1;
                    goCursor(x, 1);
                  } else {
                    goCursor(x, 1);
                  }
                }
              } else {
                if (currentRow->prev != NULL && y != 1) {
                  if (currentRow->prev->size <= x - 1) {
                    x = currentRow->prev->size + 1;
                    goCursor(x, --y);
                  } else {
                    goCursor(x, --y);
                  }
                }
              }
              break;
            case DOWN: // 아래쪽 방향키
              if (y >= row - 1 && currentRow->next != NULL) {
                p_y++;
                printf("\033[S");
                delete();
                goCursor(1, y);
                Node* printRow = head;
                for (int i = 0; i < p_y + y - 1; i++)
                  printRow = printRow->next;
                printf("%s", printRow->data);
                if (currentRow->next != NULL) {
                  if (currentRow->next->size <= x - 1) {
                    x = currentRow->next->size + 1;
                    goCursor(x, y);
                  } else
                    goCursor(x, y);
                }
              } else {
                if (currentRow->next != NULL) {
                  if (currentRow->next->size <= x - 1) {
                    x = currentRow->next->size + 1;
                    goCursor(x, ++y);
                  } else
                    goCursor(x, ++y);
                }
              }
              break;
            case LEFT: // 왼쪽 방향키
              if (currentRow->size > 1 && x > 1)
                goCursor(--x, y);
              else if (x == 1){
                if (currentRow->prev != head){
                  x = currentRow->prev->size + 1;
                  goCursor(x, --y);
                } else {
                  y = 1;
                  x = 1;
                  goCursor(x, y);
                }
              }
              break;
            case RIGHT: // 오른쪽 방향키
              if (currentRow->size >= x)
                goCursor(++x, y);
              else if (currentRow->size == x - 1 && currentRow->next == NULL){
                goCursor(x, y);
              }
              else {
                x = 1;
                goCursor(x, ++y);
              }
              break;
            case PAGEUP:
              break;
            case PAGEDOWN:
              break;
          }
        } else if (ch == '0'){
          ch = getch();
          switch (ch)
          {
            case HOME:
              x = 1;
              goCursor(x, y);
              break;
            case END:
              x = currentRow->size + 1;
              goCursor(x, y);
              break;
          }
        }
      }
#endif
      currentRow = head;
      while (currentRow-> next != NULL)
        currentRow = currentRow->next;
      setReadState(row, col, currentRow->row, argv[1]);
      goCursor(x, y);
      exitFlag = FALSE;
    }
  }
  return 0;
}
