#include <stdio.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

const int AMOUNT = 20;

int compare_ints(const void *a, const void *b)
{
    return (*((int *)a) - *((int *)b));
}

void sortAndPrint(int *mem)
{
    qsort(mem, AMOUNT, sizeof(int), compare_ints);
    for (size_t i = 0; i < AMOUNT; i++)
    {
        printf("%i ", mem[i]);
    }
    printf("\n");
}

int main()
{
    srand((unsigned)(time(0)));

    int memId = shmget(IPC_PRIVATE, sizeof(int) * AMOUNT, 0600 | IPC_CREAT | IPC_EXCL);

    int *numbers = (int *)shmat(memId, 0, 0);

    for (size_t i = 0; i < AMOUNT; i++)
    {
        numbers[i] = rand() % 10000;
    }

    for (size_t i = 0; i < AMOUNT; i++)
    {
        printf("%d ", numbers[i]);
    }

    printf("\n");

    int child_id = fork();

    if (child_id == 0)
    {
        sortAndPrint(numbers);
    }
    else
    {
        waitpid(child_id, NULL, 0);
    }

    shmdt(numbers);

    return 0;
}