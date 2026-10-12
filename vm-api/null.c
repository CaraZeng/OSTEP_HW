#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *p = NULL;
	
	printf("p = %p\n", (void *)p);
	
	printf("value = %d\n", *p);

	return 0;
}
