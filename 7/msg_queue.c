#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct thread_args
{
  size_t msgId;
};

struct my_msg
{
  long mtype;
  char msg[4];
};

void print_nums(char *nums, int c)
{
  for (int i = 0; i < c; i++)
  {
    printf("%d ", nums[i]);
  }
  printf("\n");
}

void *mainTask(void *thread_args)
{
  struct thread_args *args = (struct thread_args *)thread_args;
  size_t msgId = args->msgId;
  struct my_msg send;
  struct my_msg recive;

  char nums[4];
  srand((unsigned)(time(0)));
  for (int i = 0; i < 4; i++)
  {
    nums[i] = rand() % 10;
  }
  print_nums(nums, 4);
  for (int i = 0; i < 4; i++)
  {
    send.msg[i] = nums[i];
  }
  send.mtype = 1;
  msgsnd(msgId, &send, sizeof(send), 0);

  msgrcv(msgId, &recive, sizeof(recive), 2, 0);
  int p_count = recive.msg[0];
  printf("Permutations:\n");
  for (int i = 0; i < p_count; i++)
  {
    msgrcv(msgId, &recive, sizeof(recive), 2, 0);
    print_nums(recive.msg, 4);
  }

  printf("Permutations count:%d\n", p_count);

  msgctl(msgId, IPC_RMID, NULL);

  return NULL;
}

void *childTask(void *thread_args)
{
  struct thread_args *args = (struct thread_args *)thread_args;
  size_t msgId = args->msgId;
  struct my_msg send;
  struct my_msg recive;

  msgrcv(msgId, &recive, sizeof(recive), 1, 0);

  char p[24][4];
  int p_count = 0;
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      if (j == i)
        continue;
      for (int k = 0; k < 4; k++)
      {
        if (k == i || k == j)
          continue;
        int l = 6 - i - j - k;
        char curr_p[4];
        curr_p[0] = recive.msg[i];
        curr_p[1] = recive.msg[j];
        curr_p[2] = recive.msg[k];
        curr_p[3] = recive.msg[l];
        char unique = 1;
        for (int m = 0; m < p_count; m++)
        {
          if (memcmp(p[m], curr_p, 4) == 0)
          {
            unique = 0;
            break;
          }
        }
        if (unique)
        {
          memcpy(p[p_count], curr_p, 4);
          p_count++;
        }
      }
    }
  }
  send.msg[0] = p_count;
  send.mtype = 2;
  msgsnd(msgId, &send, sizeof(send), 0);
  for (int i = 0; i < p_count; i++)
  {
    memcpy(send.msg, p[i], 4);
    send.mtype = 2;
    msgsnd(msgId, &send, sizeof(send), 0);
  }

  return NULL;
}

int main()
{
  int msgId = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);

  struct thread_args *args;
  args->msgId = msgId;

  pthread_t mainThread, childThread;

  pthread_create(&mainThread, NULL, mainTask, (void *)args);
  pthread_create(&childThread, NULL, childTask, (void *)args);

  pthread_join(mainThread, NULL);
}