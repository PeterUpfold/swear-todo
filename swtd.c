#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

typedef struct swtodo {
	int flags;
	char * title;
} swtodo_t;


int main(int argc, char * argv[]) {


	swtodo_t *mytodos = (swtodo_t*)malloc(sizeof(swtodo_t *) * argc);
	assert(mytodos != NULL);

	for (int i = 0; i < argc; i++) {
		swtodo_t *mytodo;

		mytodo = malloc(sizeof(swtodo_t));

		assert(mytodo != NULL);

		mytodo->flags = 0;
		mytodo->title = argv[i];

		mytodos[i] = *mytodo;
	}

	for (int i = 0; i < argc; i++) {
		printf("mytodo %d is %s\n", i, mytodos[i].title);
	}

	return 0;
}
