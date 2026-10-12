#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *data = malloc(100 * sizeof(int));
	
	if (data == NULL) {
		return 1;
	}

	data[50] = 42;
	
	free(&data[50]);

	return 0;
}
