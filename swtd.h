#define SWTD_NOID -1

void new_pressed();
void edit_pressed(const char * item_name, ITEM * item, int index);
void trace_output(const char * trace_string);
ITEM ** build_menu_items();
void tidy_menu();
void build_refreshed_menu();
void populate_list_from_db();