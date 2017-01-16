#include "syscall.h"

void print(char c, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		PutChar((char)(c+i));
	}
	PutChar('\n');
}

void aff(void * x)
{
	int i = 1;
	while(i <= 10){
		i++;
		PutInt((int) x);
		PutString("\n");
	}
}

int
main()
{

	//int maxThread = 20;
	int i;

	/*for(i = 1; i<maxThread; i++){
		UserThreadCreate(aff, (void *) i);
		if(i > 1){
			UserThreadJoin(i-1);
		}
	}*/

	i = 1;

	UserThreadCreate(aff, (void *) i);
	i++;
	UserThreadCreate(aff, (void *) i);
	i++;
	UserThreadCreate(aff, (void *) i);
	i++;
	UserThreadJoin(2);
	UserThreadJoin(1);
	UserThreadJoin(3);
	PutString("------------------------\n");
	UserThreadCreate(aff, (void *) i);
	PutString("------------------------\n");

	return 0;
}
