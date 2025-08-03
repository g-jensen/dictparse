#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEADWORD 512

int b64_index(char c) {
  char base64[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int i = 0;
  while (i < 64 && c != base64[i])
    i++;
  return i;
}

int b64_to_b10(char *str) {
  int decimal = 0;
  int mult = 1;
  for (int i = strlen(str)-1; i >= 0; i--) {
    decimal += mult * b64_index(str[i]);
    mult = mult * 64;
  }
  return decimal;
}

char* create_definition(FILE* file, int byte_offset, int num_bytes) {
  fseek(file, byte_offset, SEEK_SET);
  char* buf = (char*)calloc(num_bytes+1,sizeof(char));
  fread(buf, 1, num_bytes, file);
  return buf;
}

FILE* try_fopen(char* filename, char* modes) {
  FILE *file = fopen(filename, modes);
  if (!file) {
    printf("Failed to open file: %s\n",filename);
  }
  return file;
}

void parse_until(FILE* file, char* destination, char delimiter) {
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

void parse_headword(FILE* file, char* destination) {
  parse_until(file,destination,'\t');
}

void parse_b64_offset(FILE* file, char* destination) {
  parse_until(file,destination,'\t');
}

void parse_b64_size(FILE* file, char* destination) {
  parse_until(file,destination,'\n');
}

void parse_headword_index(FILE* file, char* headword, char* b64_offset, char* b64_size) {
  parse_headword(file,headword);
  parse_b64_offset(file,b64_offset);
  parse_b64_size(file,b64_size);
}

void str_replace(char* str, char target, char replacement) {
  size_t len = strlen(str);
  for (int i = 0; i < len; i++) {
    if (str[i] == target) {
      str[i] = replacement;
    }
  }
}

void str_remove_trail(char* str, char trail) {
  size_t len = strlen(str);
  for (int i = len-1; i >= 0; i--) {
    if (str[i] == trail) {
      str[i] = '\0';
    } else {
      return;
    }
  }
}

char* create_formatted_definition(FILE* file, char* b64_offset, char* b64_size) {
  int byte_offset = b64_to_b10(b64_offset);
  int num_bytes = b64_to_b10(b64_size);
  char* definition = create_definition(file,byte_offset,num_bytes);
  str_remove_trail(definition, '\n');
  str_replace(definition,'\n',',');
  return definition;
}

void generate_definitions(FILE* dict_file, FILE* index_file, FILE* output_file, int word_count) {
  char headword[MAX_HEADWORD+1];
  char b64_offset[16];
  char b64_size[16];

  for (int k = 0; k < word_count; k++) {
    parse_headword_index(index_file,headword,b64_offset,b64_size);
    char* definition = create_formatted_definition(dict_file,b64_offset,b64_size);
    fprintf(output_file, "%s\n",definition);
    free(definition);
  }
}

int main(int argc, char *argv[]) {
  if (argc <= 4) {
    printf("Usage: %s [DICT FILE] [INDEX FILE] [OUTPUT FILE] [WORDCOUNT] - Writes the first [WORDCOUNT] definitions from [DICT FILE] to [OUTPUT FILE] with exactly one definition per newline\n",argv[0]);
    return 0;
  }
  char* dict_filename = argv[1];
  char* index_filename = argv[2];
  char* output_filename = argv[3];
  int word_count = atoi(argv[4]);

  FILE* dict_file = try_fopen(dict_filename, "r");
  if (!dict_file) return 1;
  FILE* index_file = try_fopen(index_filename, "r");
  if (!index_file) return 1;
  FILE* output_file = try_fopen(output_filename, "w");
  if (!output_file) return 1;

  generate_definitions(dict_file,index_file,output_file,word_count);

  fclose(dict_file);
  fclose(index_file);
  fclose(output_file);

  return 0;
}