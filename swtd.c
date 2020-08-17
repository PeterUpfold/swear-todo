#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sqlite3.h>

#include <curses.h>
#include <menu.h>

#include "swtd.h"
#include "swtd_strings.h"

typedef struct swtodo {
	int id;
	int flags;
	char * title;
} swtodo_t;

typedef struct swtodo_list {
	swtodo_t * todo;
	struct swtodo_list * next;
} swtodo_list_t;

MENU *swtd_menu;
swtodo_list_t *todo_list;
sqlite3* db;

/**
 * Application entry point
 * 
 * @return exit code
 */
int main(int argc, char * argv[]) {

	// open database
	int db_open_return = sqlite3_open("swtd.db", &db);
	if (db_open_return) {
		fprintf(stderr, "Failed to open database ./swtd.db");
		return db_open_return;
	}

	populate_list_from_db();

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
						edit_pressed(item_name(items[i]), items[i], i-1 /* excl. NEW */);
					}
				}
			}
			break;
		}
	}

	sqlite3_close(db);
	tidy_menu();
	endwin();
	return 0;
}

/**
 * Detect whether non-printable characters are included within
 * the string
 */
int contains_nonprintables(char * new_name) {
	//char * strpointer = new_name;
	do {
		if (!isprint((int)*new_name)) {
			return 1;	
		}
		new_name++;
	} while (*new_name != '\0');
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
	mytodo->id = SWTD_NOID;
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

	build_refreshed_menu();
}

/**
 * Rebuild the menu with new items, cleaning the old
 * one and forcing a screen refresh.
 */
void build_refreshed_menu() {
	ITEM ** our_menu_items;
	our_menu_items = build_menu_items();

	tidy_menu(swtd_menu);
	swtd_menu = new_menu((ITEM **)our_menu_items);
	refresh();
	post_menu(swtd_menu);
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

	// ?? \now we want to use the opaque reference in the menu item so we can track todos by ID??

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
 * Output an instruction on screen to the user.
 */
void instruction_line(const char * instruction_string) {
	mvprintw(LINES - 2, 0, instruction_string);
}

/**
 * Clean up old menu.
 */
void tidy_menu() {
	assert(swtd_menu != NULL);

	erase();

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
void edit_pressed(const char * item_name, ITEM * item, int index) {
	char new_name[128];

	assert(index >= 0);
	assert(todo_list != NULL);

	instruction_line("                         ");
	instruction_line(SWTD_RENAME);
	echo();
	getnstr(new_name, 128);
	noecho();
	instruction_line("                         ");

	if (strlen(new_name) < 1) {
		instruction_line(SWTD_RENAME_ZEROLENGTH);
		// refresh menu
		tidy_menu();
		build_refreshed_menu();
		return;
	}

	// find non-printable characters and reject?
	if (contains_nonprintables(&new_name)) {
		instruction_line(SWTD_RENAME_NONPRINTABLE);
		// refresh menu
		tidy_menu();
		build_refreshed_menu();
		return;
	}

	// find item and rename it
	swtodo_list_t *current_list_item = todo_list;
	for (int i = 0; i < index; i++) {
		current_list_item = current_list_item->next;
	}

	swtodo_t *target = current_list_item->todo;
	free(target->title);
	target->title = strdup(new_name); // when do we need to free() this?

	// refresh menu
	tidy_menu();
	build_refreshed_menu();

	trace_output(new_name);
}

/**
 * Receive a row of data from SQLite.
 */
int populate_callback(void* opaque_data, int column_count, char** result_columns, char** column_names) {
	swtodo_t *mytodo;
	mytodo = malloc(sizeof(swtodo_t));
	assert(mytodo != NULL);
	mytodo->flags = 0;
	mytodo->title = SWTD_UNTITLED;

	for (int i = 0; i < column_count; i++) {
		fprintf(stderr, "%d: %s %s\n", i, result_columns[i], column_names[i]);
		switch(i) {
			case 0:
				mytodo->id = atoi(result_columns[i]);
			break;
			case 1:
				mytodo->flags = atoi(result_columns[i]);
			break;
			case 2:
				mytodo->title = strdup(result_columns[i]);
			break;
		}
	}

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

}

/**
 * Build the global swtodo_list_t from the SQLite database.
 */
void populate_list_from_db() {
	char *err_msg = 0;

	// try to create table
	sqlite3_exec(db,
	"CREATE TABLE IF NOT EXISTS todos (id INTEGER PRIMARY KEY AUTOINCREMENT, flags INTEGER, title TEXT);",
	NULL,
	NULL,
	&err_msg
	);

	if (err_msg != NULL) {
		fprintf(stderr, "Failed to create table: %s\n", err_msg);
		sqlite3_free(err_msg); 
		err_msg = 0;
	}

	// what happens to err_msg here if it's used already, but we free()d it???

	sqlite3_exec(db,
	"SELECT * FROM todos",
	&populate_callback,
	NULL,
	&err_msg
	);

	if (err_msg != NULL) {
		fprintf(stderr, "%s\n", err_msg);
		sqlite3_free(err_msg);
		err_msg = 0;
	}
}

/**
 * Save the specified todo in the SQLite DB.
 * 
 * @return 1 if success, 0 otherwise
 */
int save_todo(swtodo_t * todo) {
	sqlite3_stmt *statement;
	char *err_msg = 0;
	char *sql = "";

	if (todo->id == SWTD_NOID) {
		// create for the first time
		sql = "INSERT INTO todos (flags, title) VALUES (?, ?)";
	}
	else {
		sql = "UPDATE todos SET flags = ?, title = ? WHERE id = ?";
	}

	if (sqlite3_prepare_v2(db, sql, -1, &statement, 0) != SQLITE_OK) {
		fprintf(stderr, "%s\n", err_msg);
		sqlite3_free(err_msg);
		err_msg = 0;
		return 0;
	}

	sqlite3_bind_int(statement, 1, todo->flags);
	sqlite3_bind_text(statement,
	2,
	todo->title,
	strlen(todo->title),
	/* number of bytes, not actually string length in case of utf-8 */
	SQLITE_STATIC /* is this correct? */
	);

	if (todo->id != SWTD_NOID) {
		sqlite3_bind_int(statement, 3, todo->id);
	}

	sqlite3_step(statement); //TODO what does the return value end up being here?
	sqlite3_finalize(statement);
	return 1;
}