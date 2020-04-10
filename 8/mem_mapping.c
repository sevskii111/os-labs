#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  const char *input_file_name = argv[1];
  const char *output_file_name = argv[2];

  int input_file = open(input_file_name, O_RDONLY, 0600);
  int output_file = open(output_file_name, O_RDWR | O_CREAT, 0600);

  struct stat st;
  stat(input_file_name, &st);
  int file_size = st.st_size;
  ftruncate(output_file, file_size);

  char *output_file_data = (char *)mmap(NULL, file_size, PROT_WRITE | PROT_READ, MAP_SHARED, output_file, 0);
  read(input_file, output_file_data, file_size);
  munmap(output_file_data, file_size);

  close(input_file);
  close(output_file);
}