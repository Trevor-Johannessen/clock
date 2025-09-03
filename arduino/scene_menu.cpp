#include "letters.h"
#include "menu.h"
#include "scene.h"

#define MENU_ICON_COUNT 3

Menu *selector;
char *icon_strings[MENU_ICON_COUNT] = {
  "Clock",
  "PPP",
  "APA"
};

void _scene_menu_button_pressed(){
}

void _scene_menu_button_released(int ms){
  if(ms < 500){
    selector->cursor++;
    if(selector->cursor >= MENU_ICON_COUNT)
      selector->cursor = 0;
  } else {
    menu_activate(selector);
  }
}

void _scene_menu_select(char icon, char *name){
  Scene *s;

  if(!(s = scene_find(name)))
    return;
  scene_switch(s);
}

void _scene_menu_stage(){
  character_write(selector->items[selector->cursor].icon, 0, 0);
  sentence_write(icon_strings[selector->cursor], 12, 0);
}

void _scene_menu_unstage(){
  character_write(selector->items[selector->cursor].icon, 0, 0);
  sentence_write(icon_strings[selector->cursor], 12, 0);
}

void _scene_menu_update(){
  const int frames = 8;
  static char prev_cursor = 0;
  char icon, prev_icon, cursor, i;
  unsigned int ms;

  // Transition with animation between icons
  if(prev_cursor != selector->cursor){
    icon = selector->items[selector->cursor].icon;
    prev_icon = selector->items[prev_cursor].icon;
    for(i=0;i<frames;i++){
      // Erase and rewrite old icon
      if(prev_icon){
        character_write(prev_icon, 0, -i);
        sentence_write(icon_strings[prev_cursor], 12, -i);
        character_write(prev_icon, 0, -i-1);
        sentence_write(icon_strings[prev_cursor], 12, -i-1);
      }
      // Write new Icon
      character_write(icon, 0, frames-i);
      sentence_write(icon_strings[selector->cursor], 12, frames-i);
      character_write(icon, 0, frames-i-1);
      sentence_write(icon_strings[selector->cursor], 12, frames-i-1);

      ms=millis();
      while(millis()<ms+50);
    }
    prev_cursor = selector->cursor;
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
  menu_register(selector, 0, 2, icon_strings[0], _scene_menu_select);
  menu_register(selector, 1, 2, icon_strings[1], _scene_menu_select);
  menu_register(selector, 2, 3, icon_strings[2], _scene_menu_select);

  return menu_scene;
}
