#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#define MAX_FILENAME_SIZE 256

int main(int argc, char *argv[])
{
  struct sockaddr_in servaddr;
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);

  char *sep = strchr(argv[1], ':');
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(sep + 1));
  sep[0] = 0;
  inet_aton(argv[1], &servaddr.sin_addr);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    printf("Can't connect to server\n");
    return 1;
  }

  printf("Ready to recive commands\n");
  char command[MAX_FILENAME_SIZE];
  while (1)
  {
    scanf("%s", command);

    if (strcmp(command, "exit") == 0)
    {
      exit(0);
    }
    else if (strcmp(command, "help") == 0)
    {
      printf("Avalible commands:\n");
      printf("exit - closes app\n");
      printf("help - shows avalible commands\n");
      printf("any other command will be interpreted as filename\n");
    }
    else
    {
      write(sockfd, command, strlen(command) + 1);
      long status;
      int n = read(sockfd, &status, sizeof(status));
      if (n == 0)
      {
        close(sockfd);
        printf("Connection lost\n");
        exit(1);
      }
      else if (status == -1)
      {
        printf("File not found by server\n");
      }
      else
      {
        printf("Downloading file\n");
        long filesize = status;
        int output_file = open(command, O_RDWR | O_CREAT, 0600);
        ftruncate(output_file, filesize);
        char *output_file_data = (char *)mmap(NULL, filesize, PROT_WRITE | PROT_READ, MAP_SHARED, output_file, 0);
        read(sockfd, output_file_data, filesize);
        munmap(output_file_data, filesize);
        close(output_file);
        printf("Downloading finished\n");
      }
    }
  }
  close(sockfd);
}