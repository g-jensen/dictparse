#include "dictparse.h"
#include "file.h"

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