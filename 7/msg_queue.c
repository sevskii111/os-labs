#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct thread_args
{
  size_t msgId;
};

struct msg
{
  long mtype;
  char msg[4];
};

void print_nums(int *nums, int c)
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
  struct msg send;
  struct msg recive;

  int nums[4];
  srand((unsigned)(time(0)));
  for (int i = 0; i < 4; i++)
  {
    nums[i] = rand() % 100;
  }
  print_nums(nums, 4);
  send.mtype = 1;
  for (int i = 0; i < 4; i++)
  {
    send.msg[i] = nums[i];
    msgsnd(msgId, &send, sizeof(send), 0);
  }
  msgctl(msgId, IPC_RMID, NULL);
}

void *childTask(void *thread_args)
{
  struct thread_args *args = (struct thread_args *)thread_args;
  size_t msgId = args->msgId;
  struct msg send;
  struct msg recive;

  int nums[4];
  msgrcv(msgId, &recive, sizeof(recive), 0, 0);
  for (int i = 0; i < 4; i++)
  {
    nums[i] = recive.msg[i];
  }

  print_nums(nums, 4);
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