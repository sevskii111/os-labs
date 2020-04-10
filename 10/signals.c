#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int sigint_c = 0, sigusr1_c = 0, sigusr2_c = 0;

void handl_sig(int nsig)
{
  switch (nsig)
  {
  case SIGINT:
    sigint_c++;
    break;

  case SIGUSR1:
    sigusr1_c++;
    break;

  case SIGUSR2:
    sigusr2_c++;
    break;

  default:
    break;
  }
}

int main()
{
  signal(SIGTERM, SIG_IGN);
  signal(SIGINT, handl_sig);
  signal(SIGUSR1, handl_sig);
  signal(SIGUSR2, handl_sig);
  while (1)
  {
    size_t startTime = time(NULL);
    while (time(NULL) - startTime < 1 && sigint_c < 5)
      usleep(100);
    printf("Time: %ld\n", time(NULL));
    printf("SIGINT: %d\n", sigint_c);
    printf("SIGUSR1: %d\n", sigusr1_c);
    printf("SIGUSR2: %d\n", sigusr2_c);
    if (sigint_c >= 5)
      break;
  }
}