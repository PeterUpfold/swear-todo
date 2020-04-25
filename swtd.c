#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <curses.h>
#include <menu.h>


#include "swtd_strings.h"

typedef struct swtodo {
	int flags;
	char * title;
} swtodo_t;


int main(int argc, char * argv[]) {


	ITEM **our_menu_items;
	MENU *swtd_menu;
	int current_char = -1;
	ITEM **items;

	initscr();
	start_color();
	cbreak();

	noecho();

	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);

	our_menu_items = (ITEM **)calloc(3 /* size of plus one null item */, sizeof(ITEM *));

	our_menu_items[0] = new_item(SWTD_NEW, SWTD_NEW);
	our_menu_items[1] = new_item(SWTD_EDIT, SWTD_EDIT);
	our_menu_items[2] = (ITEM *)NULL;

	swtd_menu = new_menu((ITEM **)our_menu_items);

	mvprintw(LINES - 2, 0, SWTD_F1_TO_EXIT);

	post_menu(swtd_menu);
	refresh();

	while ((current_char != KEY_F(1))) {
		current_char = getch();
		switch(current_char) {
			case KEY_DOWN:
			menu_driver(swtd_menu, REQ_DOWN_ITEM);
			break;
			case KEY_UP:
			menu_driver(swtd_menu, REQ_UP_ITEM);
			break;
			case 10: /* Enter */

			items = menu_items(swtd_menu);

			for (int i = 0; i < item_count(swtd_menu); i++) {
				//printf("loop over %s\n", item_name(items[i]));

				if (current_item(swtd_menu) == items[i]) {
					if (strcmp(item_name(items[i]), SWTD_NEW) == 0) {
						// handle new item

					}
					else if (strcmp(item_name(items[i]), SWTD_EDIT) == 0) {
						// handle edit
						
					}
				}
			}
			break;
		}
	}

	free_item(our_menu_items[0]);
	free_item(our_menu_items[1]);
	free_item(our_menu_items[2]);
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
