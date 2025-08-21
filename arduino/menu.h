#ifndef MENU_H
#define MENU_H

#include <stdlib.h>

typedef struct menu_item {
    char icon;
    char *display_text;
    void (*func)(char, char *);
} MenuItem;

typedef struct menu {
    char width;
    char cursor;
    char size; // count of how many items are in the window
    char window_start;
    MenuItem items[];
} Menu;


Menu *menu_create(char width, char size);
void menu_free(Menu *menu, char spare_strings);
void menu_select(Menu *menu, char pos);
void menu_activate(Menu *menu);
void menu_register(Menu *menu, char i, char icon, char *display, void (*func)(char, char *));
char *menu_window(Menu *menu);
char *menu_message(Menu *menu);

#endif