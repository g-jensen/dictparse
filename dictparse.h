#ifndef DICTPARSE
#define DICTPARSE

#define MAX_HEADWORD 512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char headword[MAX_HEADWORD+1];
  int offset;
  int size;
} definition_index;

void parse_headword_index(FILE* file, definition_index* index);
void create_definition(FILE* file, definition_index index, char* destination);
void create_formatted_definition(FILE* file, definition_index index, char* destination);
void generate_definitions(FILE* dict_file, FILE* index_file, FILE* output_file, int word_count);

#endif