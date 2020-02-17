#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>

int compare_ints(const void* a, const void* b)
{
	return ( *((int*)a) - *((int*)b) );
}

int main(int argv, char* argc[])
{	
	int memId = atoi(argc[1]);
	const int AMOUNT = atoi(argc[2]);
	
	int* mem = (int*)shmat(memId, 0, 0);

	for (size_t i = 0;i < AMOUNT;i++) 
	{
		printf("%i ", mem[i]);
	}
	printf("\n");

	qsort(mem, AMOUNT, sizeof(int), compare_ints);
	for (size_t i = 0;i < AMOUNT;i++) 
	{
		printf("%i ", mem[i]);
	}
	printf("\n");

	return 0;
}
