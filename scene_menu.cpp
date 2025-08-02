#include "letters.h"
#include "menu.h"
#include "scene.h"

#define MENU_ICON_COUNT 2

Menu *selector;
char *icon_strings[MENU_ICON_COUNT] = {
  "Clock",
  "Placeholder"
};

void _scene_menu_button_pressed(){
}

void _scene_menu_button_released(int ms){
}

void _scene_menu_select(char icon, char *name){
  Scene *s;

  if(!(s = scene_find(name))){
    Serial.printf("Could not find scene '%s'.\n", name);
    return;
  }
  scene_switch(s);
}

void _scene_menu_stage(){
  character_write(0, 0, selector->items[selector->cursor].icon);
}

void _scene_menu_unstage(){
  character_write(0, 0, selector->items[selector->cursor].icon);
}

void _scene_menu_update(){
  static char prev_icon = '\0';
  char icon;

  icon = selector->items[selector->cursor].icon;
  if(prev_icon != icon){
    if(prev_icon)
      character_write(0,0, prev_icon);
    character_write(0,0, icon);
    prev_icon = icon;
  }
}

Scene *scene_menu(){
  Scene *menu_scene;

  // initalize scene
  menu_scene = scene_create("Menu");
  menu_scene->stage = _scene_menu_stage;
  menu_scene->unstage = _scene_menu_unstage;
  menu_scene->update = _scene_menu_update;
  menu_scene->button_pressed = _scene_menu_button_pressed;
  menu_scene->button_released = _scene_menu_button_released;

  selector = menu_create(1, MENU_ICON_COUNT);
  menu_register(selector, 0, 'A', icon_strings[0], _scene_menu_select);
  menu_register(selector, 1, 'P', icon_strings[1], _scene_menu_select);

  return menu_scene;
}
