#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int *data;
	int size;
} Vector;

void add(Vector *v, int value)
{
	v->size++;
	
	int *temp = realloc(v-> data, v->size * sizeof(int));

	if (temp == NULL) {
		printf("realloc failed\n");
		free(v->data);
		exit(1);
	}

	v->data = temp;
	v->data[v->size - 1] = value;
}

int main()
{
	Vector v;
	v.data = NULL;
	v.size = 0;
	
	for (int i = 0; i < 10; i++) {
		add(&v, i * 10);
	}

	for (int i = 0; i < v.size; i++) {
		printf("%d ", v.data[i]);
	}

	printf("\n");

	free(v.data);
	
	return 0;
}
