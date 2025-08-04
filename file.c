#include "file.h"

FILE* try_fopen(char* filename, char* modes) {
  FILE *file = fopen(filename, modes);
  if (!file) {
    printf("Failed to open file: %s\n",filename);
  }
  return file;
}