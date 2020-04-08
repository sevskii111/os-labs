#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>

void sem(int semId, int n, int d)
{
    struct sembuf op;
    op.sem_op = d;
    op.sem_flg = 0;
    op.sem_num = n;
    semop(semId, &op, 1);
}

void unlockSem(int semId, int checkMemId, int n)
{
    int *check = (int *)shmat(checkMemId, 0, 0);
    sem(semId, n, 1);
    check[n] = 0;
}

int lockSem(int semId, int checkMemId, int n)
{
    int *check = (int *)shmat(checkMemId, 0, 0);
    int isBusy = check[n];
    sem(semId, n, -1);
    check[n] = 1;
    return isBusy;
}

void sort(int semId, int memId, int checkMemId, const size_t n)
{
    int *nums = (int *)shmat(memId, 0, 0);

    for (int i = 0; i < n; i++)
    {
        int minInd = i;
        for (int j = i + 1; j < n; j++)
        {
            lockSem(semId, checkMemId, i);
            lockSem(semId, checkMemId, j);
            if (nums[j] < nums[minInd])
            {
                minInd = j;
            }
            unlockSem(semId, checkMemId, i);
            unlockSem(semId, checkMemId, j);
        }
        if (i != minInd)
        {
            lockSem(semId, checkMemId, i);
            lockSem(semId, checkMemId, minInd);
            int t = nums[i];
            nums[i] = nums[minInd];
            nums[minInd] = t;
            unlockSem(semId, checkMemId, i);
            unlockSem(semId, checkMemId, minInd);
        }
    }
}

void fill_random_nums(int *nums, int n, int min, int max)
{
    srand((unsigned)(time(0)));
    for (int i = 0; i < n; i++)
    {
        nums[i] = min + rand() % (max - min + 1);
    }
}

void print_nums(int *nums, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", nums[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    const int N = atoi(argv[1]);
    const int min = atoi(argv[2]);
    const int max = atoi(argv[3]);

    int memId = shmget(IPC_PRIVATE, sizeof(int) * N, 0600 | IPC_CREAT | IPC_EXCL);
    int checkMemId = shmget(IPC_PRIVATE, sizeof(int) * N, 0600 | IPC_CREAT | IPC_EXCL);
    int semId = semget(IPC_PRIVATE, N, 0600 | IPC_CREAT);

    int *numbers = (int *)shmat(memId, 0, 0);
    fill_random_nums(numbers, N, min, max);
    print_nums(numbers, N);

    for (int i = 0; i < N; i++)
    {
        unlockSem(semId, checkMemId, i);
    }

    int childId = fork();
    if (childId == 0)
    {
        sort(semId, memId, checkMemId, N);
    }
    else
    {
        int i = 0;
        int status;
        do
        {
            printf("%d: ", i);
            for (int j = 0; j < N; j++)
            {
                if (lockSem(semId, checkMemId, j))
                {
                    printf("[%d] ", numbers[j]);
                }
                else
                {
                    printf("%d ", numbers[j]);
                }

                fflush(stdout);
                unlockSem(semId, checkMemId, j);
            }
            printf("\r\n");
            status = waitpid(childId, NULL, WNOHANG);
            i++;
        } while (!status);

        printf("Sort finished \r\n");
        print_nums(numbers, N);

        shmctl(memId, 0, IPC_RMID);
        shmctl(checkMemId, 0, IPC_RMID);
        semctl(semId, 0, IPC_RMID);
    }
}