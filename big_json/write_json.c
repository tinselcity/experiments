#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define FATAL(...) do { fprintf(stderr, __VA_ARGS__); return -1;} while(0)

// ref: https://codereview.stackexchange.com/a/29200
static char *rand_string(char *str, size_t size)
{
  const char charset[] = "abcdefghijklmnopqrstuvwxyz";
  if (size) {
    --size;
    for (size_t n = 0; n < size; n++) {
      int key = rand() % (int) (sizeof charset - 1);
      str[n] = charset[key];
    }
    str[size] = '\0';
  }
  return str;
}

int main(int argc, char **argv) {
  FILE* fp = fopen(argv[1], "w+");
  if (fp == NULL) {
    FATAL("opening file: %s. Reason: %s\n", argv[1], strerror(errno));
  }
  unsigned long num = strtoul(argv[2], NULL, 10);
  fwrite("{ \"customers\": [", strlen("{ \"customers\": ["), 1, fp);
  // "balance": rand -10,000 -> 1,0000,0000
#define _BALANCE_MIN -10000
#define _BALANCE_MAX 1000000
  char name[17] = {'\0'};
  srand(time(NULL));
  for (unsigned long i = 0; i < num; ++i) {
    int balance = _BALANCE_MIN +
                  rand() / (RAND_MAX / (_BALANCE_MAX - _BALANCE_MIN + 1) + 1);
    fprintf(fp, "{\"name\": \"%s\", \"balance\": %d}", rand_string(name, 16), balance);
    if (i+1 < num) {
      fprintf(fp, ",");
    }
  }
  fwrite("]}", 2, 1, fp);
  fclose(fp);
  return 0;
}
