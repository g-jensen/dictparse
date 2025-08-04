#include "dictparse.h"

static int b64_index(char c) {
  char base64[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int i = 0;
  while (i < 64 && c != base64[i])
    i++;
  return i;
}

static int b64_to_b10(char *str) {
  int decimal = 0;
  int mult = 1;
  for (int i = strlen(str)-1; i >= 0; i--) {
    decimal += mult * b64_index(str[i]);
    mult = mult * 64;
  }
  return decimal;
}

void create_definition(FILE* file, definition_index index, char* destination) {
  fseek(file, index.offset, SEEK_SET);
  fread(destination, 1, index.size, file);
}

static void parse_until(FILE* file, char* destination, char delimiter) {
  int i = 0;
  char c;
  fread(&c, 1, 1, file);
  while (c != EOF && c != delimiter) {
    destination[i] = c;
    i++;
    fread(&c, 1, 1, file);
  }
  destination[i] = '\0';
}

static void parse_headword(FILE* file, char* destination) {
  parse_until(file,destination,'\t');
}

static void parse_b64_offset(FILE* file, int* destination) {
  char b64_offset[16];
  parse_until(file,b64_offset,'\t');
  *destination = b64_to_b10(b64_offset);
}

static void parse_b64_size(FILE* file, int* destination) {
  char b64_size[16];
  parse_until(file,b64_size,'\n');
  *destination = b64_to_b10(b64_size);
}

void parse_headword_index(FILE* file, definition_index* index) {
  parse_headword(file,index->headword);
  parse_b64_offset(file,&index->offset);
  parse_b64_size(file,&index->size);
}

static void str_replace(char* str, char target, char replacement) {
  size_t len = strlen(str);
  for (size_t i = 0; i < len; i++) {
    if (str[i] == target) {
      str[i] = replacement;
    }
  }
}

static void str_remove_trail(char* str, char trail) {
  size_t len = strlen(str);
  for (int i = len-1; i >= 0; i--) {
    if (str[i] == trail) {
      str[i] = '\0';
    } else {
      return;
    }
  }
}

void create_formatted_definition(FILE* file, definition_index index, char* destination) {
  create_definition(file,index,destination);
  str_remove_trail(destination, '\n');
  str_replace(destination,'\n',',');
}

void generate_definitions(FILE* dict_file, FILE* index_file, FILE* output_file, int word_count) {
  definition_index index;
  for (int k = 0; k < word_count; k++) {
    parse_headword_index(index_file,&index);
    char* definition = (char*)calloc(index.size+1,sizeof(char));
    create_formatted_definition(dict_file,index,definition);
    fprintf(output_file, "%s\n",definition);
    free(definition);
  }
}