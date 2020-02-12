#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <curses.h>
#include <menu.h>

typedef struct swtodo {
	int flags;
	char * title;
} swtodo_t;


int main(int argc, char * argv[]) {


	ITEM **menu_items;
	MENU *swtd_menu;

	initscr();
	cbreak();

	noecho();

	keypad(stdscr, TRUE);

	menu_items = (ITEM **)calloc(2 /* size of plus one null item */, sizeof(ITEM *));

	menu_items[0] = new_item("New", "New");
	menu_items[1] = (ITEM *)NULL;

	swtd_menu = new_menu((ITEM **)menu_items);
	post_menu(swtd_menu);
	refresh();

	free_item(menu_items[0]);
	free_item(menu_items[1]);
	free_menu(swtd_menu);
	endwin();

	/*swtodo_t *mytodos = (swtodo_t*)malloc(sizeof(swtodo_t *) * argc);
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
	}*/

	return 0;
}
