#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#ifdef __linux__
#define UDP_BUFFER_LEN 65507
#elif __APPLE__
#define UDP_BUFFER_LEN 9216
#endif

int compare_ints(const void *a, const void *b)
{
  return (*((int *)b) - *((int *)a));
}

int main(int argc, char *argv[])
{
  size_t maxlen = UDP_BUFFER_LEN;
  int sockfd;
  char line[maxlen];
  struct sockaddr_in servaddr, cliaddr;

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[1]));
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  sockfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    servaddr.sin_port = 0;
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
      perror(NULL);
      close(sockfd);
      exit(1);
    }
  }
  socklen_t servlen = sizeof(servaddr);
  getsockname(sockfd, (struct sockaddr *)&servaddr, &servlen);
  printf("Listening on port: %d\n", ntohs(servaddr.sin_port));

  while (1)
  {
    socklen_t clilen = sizeof(cliaddr);
    int n = recvfrom(sockfd, line, maxlen, 0, (struct sockaddr *)&cliaddr, &clilen);
    qsort(line, n * sizeof(char) / sizeof(int), sizeof(int), compare_ints);
    sendto(sockfd, line, n, 0, (struct sockaddr *)&cliaddr, clilen);
  }
}