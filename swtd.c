#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <curses.h>
#include <menu.h>

#include "swtd.h"
#include "swtd_strings.h"

typedef struct swtodo {
	int flags;
	char * title;
} swtodo_t;

typedef struct swtodo_list {
	swtodo_t * todo;
	struct swtodo_list * next;
} swtodo_list_t;

MENU *swtd_menu;
swtodo_list_t *todo_list;

int main(int argc, char * argv[]) {


	ITEM **our_menu_items;
	int current_char = -1;
	ITEM **items;

	initscr();
	start_color();
	cbreak();

	noecho();

	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);

	our_menu_items = build_menu_items();

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
					if (strncmp(item_name(items[i]), SWTD_NEW, sizeof(item_name(items[i]))) == 0) {
						// handle new item
						new_pressed();
					}
					else {
						// handle edit
						edit_pressed(item_name(items[i]), items[i]);
					}
				}
			}
			break;
		}
	}

	tidy_menu();
	endwin();
	return 0;
}

/*
 * Handle the pressing of the new item menu item.
 * 
 */
void new_pressed() {
	// new item 
	ITEM *the_new_item;
	the_new_item = new_item(SWTD_UNTITLED, SWTD_UNTITLED);

	swtodo_t *mytodo;
	mytodo = malloc(sizeof(swtodo_t));
	assert(mytodo != NULL);
	mytodo->flags = 0;
	mytodo->title = SWTD_UNTITLED;

	// init todo_list
	if (todo_list == NULL) {
		todo_list = malloc(sizeof(swtodo_list_t));
		assert(todo_list != NULL);
		todo_list->todo = mytodo;
		todo_list->next = NULL;
	}
	else {
		// find the end of todo_list by traversing through
		swtodo_list_t *current_list_item = todo_list;
		while (current_list_item->next != NULL) {
			current_list_item = current_list_item->next;
		}
		
		// create a new list item -- set next of previous to that item
		swtodo_list_t *new_list_item = malloc(sizeof(swtodo_list_t));
		assert(new_list_item != NULL);

		new_list_item->todo = mytodo;
		new_list_item->next = NULL;
		current_list_item->next = new_list_item;
	}	

	ITEM ** our_menu_items;
	our_menu_items = build_menu_items();

	tidy_menu(swtd_menu);
	swtd_menu = new_menu((ITEM **)our_menu_items);
	refresh();
	post_menu(swtd_menu);

	//trace_output("New item spawned");
}

/**
 * Build menu items from current todo list.
 */
ITEM ** build_menu_items() {
	ITEM **our_menu_items;

	int item_count = 0;

	if (todo_list != NULL) {
		swtodo_list_t *current_item = todo_list;
		do {
			current_item = current_item->next;
			item_count++;
		} while (current_item != NULL);
	}

	char trace_string[32];
	snprintf(trace_string, 32, "item count: %d", item_count);
	trace_output(trace_string);

	our_menu_items = (ITEM **)calloc(item_count + 2 /* size of plus one null item */, sizeof(ITEM *));

	our_menu_items[0] = new_item(SWTD_NEW, SWTD_NEW);
	//our_menu_items[1] = new_item(SWTD_EDIT, SWTD_EDIT);

	int i = 0;
	swtodo_list_t *current_item = todo_list;
	for (i = 0; i < item_count; i++) {
		// follow the pointers from todo_list
		assert(current_item != NULL);

		swtodo_t *current_todo = current_item->todo;
		assert(current_todo != NULL);
		
		our_menu_items[i+1] = new_item(current_todo->title, current_todo->title);

		current_item = current_item->next;
	}

	our_menu_items[i+1] = (ITEM *)NULL;
	return our_menu_items;
}

/**
 * Output tracing information
 */
void trace_output(const char * trace_string) {
		mvprintw(LINES - 3, 0, trace_string);
}

/**
 * Clean up old menu.
 */
void tidy_menu() {
	assert(swtd_menu != NULL);

	ITEM ** items = menu_items(swtd_menu);
	for (int i = 0; i < item_count(swtd_menu); i++) {
		assert(items[i] != NULL);
		free_item(items[i]);
	}

	free_menu(swtd_menu);
}

/**
 * Handle pressing the edit button.
 */
void edit_pressed(const char * item_name, ITEM * item) {
	char new_name[128];

	getnstr(new_name, 128);

	trace_output(new_name);
}