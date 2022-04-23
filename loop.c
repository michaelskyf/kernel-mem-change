#include <stdio.h>

int main()
{
	volatile int always_true = 1;

	printf("Hello, stranger!\n");

	/* Simple while loop */
	while(always_true){}

	printf("WHAT HAVE YOU DONE?!?!\n");

	return 0;
}
