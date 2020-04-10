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
  size_t msgId1;
  size_t msgId2;
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
  size_t msgId1 = args->msgId1;
  size_t msgId2 = args->msgId2;
  struct my_msg send;
  struct my_msg recive;

  char nums[4];
  srand((unsigned)(time(0)));
  for (int i = 0; i < 4; i++)
  {
    nums[i] = rand() % 10;
  }
  print_nums(nums, 4);
  send.mtype = 1;
  for (int i = 0; i < 4; i++)
  {
    send.msg[i] = nums[i];
  }
  msgsnd(msgId1, &send, sizeof(send), 0);

  msgrcv(msgId2, &recive, sizeof(recive), 0, 0);
  int p_count = recive.msg[0];
  printf("Pemutations:\n");
  for (int i = 0; i < p_count; i++)
  {
    msgrcv(msgId2, &recive, sizeof(recive), 0, 0);
    print_nums(recive.msg, 4);
  }

  printf("Pemutations count:%d\n", p_count);

  msgctl(msgId1, IPC_RMID, NULL);
  msgctl(msgId2, IPC_RMID, NULL);

  return NULL;
}

void *childTask(void *thread_args)
{
  struct thread_args *args = (struct thread_args *)thread_args;
  size_t msgId1 = args->msgId1;
  size_t msgId2 = args->msgId2;
  struct my_msg send;
  struct my_msg recive;

  char nums[4];

  msgrcv(msgId1, &recive, sizeof(recive), 0, 0);

  for (int i = 0; i < 4; i++)
  {
    nums[i] = recive.msg[i];
  }

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
        curr_p[0] = nums[i];
        curr_p[1] = nums[j];
        curr_p[2] = nums[k];
        curr_p[3] = nums[l];
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
  send.mtype = 2;
  send.msg[0] = p_count;
  msgsnd(msgId2, &send, sizeof(send), 0);
  send.mtype = 1;
  for (int i = 0; i < p_count; i++)
  {
    memcpy(send.msg, p[i], 4);

    msgsnd(msgId2, &send, sizeof(send), 0);
  }

  return NULL;
}

int main()
{
  int msgId1 = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);
  int msgId2 = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);

  struct thread_args *args;
  args->msgId1 = msgId1;
  args->msgId2 = msgId2;

  pthread_t mainThread, childThread;

  pthread_create(&mainThread, NULL, mainTask, (void *)args);
  pthread_create(&childThread, NULL, childTask, (void *)args);

  pthread_join(mainThread, NULL);
}