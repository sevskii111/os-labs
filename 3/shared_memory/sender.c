#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>

const int AMOUNT = 20;

int main()
{
	srand((unsigned)(time(0)));
		
	int memId = shmget(IPC_PRIVATE, sizeof(int) * AMOUNT, 0600|IPC_CREAT|IPC_EXCL);
	
	int* numbers = (int*)shmat(memId, 0, 0);

	for (size_t i = 0;i < AMOUNT;i++) 
	{
			numbers[i] = rand() % 10000;
	}
	
	char callbuf[1024];
	sprintf(callbuf, "./receiver %i %i", memId, AMOUNT);
	system(callbuf);
	
	return 0;
}
