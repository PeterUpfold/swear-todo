#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

typedef struct swtodo {
	int flags;
	char * title;
} swtodo_t;


int main() {

	swtodo_t *mytodo;

	mytodo = malloc(sizeof(swtodo_t));

	assert(mytodo != NULL);

	mytodo->flags = 0;
	mytodo->title = "title";

	return 0;
}
