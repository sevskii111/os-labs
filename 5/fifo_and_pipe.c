#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

const char *FIFO_NAME = "novsu.fifo";

int compare_ints(const void *a, const void *b)
{
    return (*((int *)b) - *((int *)a));
}

int *get_random_nums(int n)
{
    srand((unsigned)(time(0)));
    int *nums = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
    {
        nums[i] = rand() % 100000;
    }
    return nums;
}

void print_nums(int *nums, int c)
{
    for (int i = 0; i < c; i++)
    {
        printf("%d ", nums[i]);
    }
    printf("\n");
}

int main(int argv, char *argc[])
{
    int n = atoi(argc[1]);
    int *nums = get_random_nums(n);
    print_nums(nums, n);

    mknod(FIFO_NAME, S_IFIFO | 0666, 0);
    int p[2];
    pipe(p);

    int child_id = fork();

    if (child_id == 0)
    {
        close(p[0]);
        int fifo = open(FIFO_NAME, O_RDONLY);
        int *received_nums = malloc(sizeof(int) * n);
        read(fifo, received_nums, sizeof(int) * n);
        close(fifo);
        qsort(received_nums, n, sizeof(int), compare_ints);
        write(p[1], received_nums, sizeof(int) * n);
        close(p[1]);
    }
    else
    {
        close(p[1]);
        int fifo = open(FIFO_NAME, O_WRONLY);
        write(fifo, nums, sizeof(int) * n);
        close(fifo);
        int *sorted_nums = malloc(sizeof(int) * n);
        read(p[0], sorted_nums, sizeof(int) * n);
        print_nums(sorted_nums, n);
        close(p[0]);
        unlink(FIFO_NAME);
    }
}