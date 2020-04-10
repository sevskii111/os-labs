#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  const char *fileName = argv[1];
  int byteC = atoi(argv[2]);

  FILE *fin = fopen(fileName, "r");
  char data[byteC];
  int res = fread(data, sizeof(char), byteC, fin);

  printf("%d\n", res);
  for (int i = 0; i < res; i++)
  {
    printf("%c[%d]\n", data[i], data[i]);
  }
  printf("\n");

  fclose(fin);
}