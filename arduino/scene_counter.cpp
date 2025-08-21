#include <stdlib.h>
#include "letters.h"
#include "scene.h"

static int clicks = 0;
static char *clicks_str = 0x0;

void _scene_counter_write_count();

void _scene_counter_button_pressed(){
}

void _scene_counter_button_released(int ms){
  // Erase last number
  _scene_counter_write_count();

  // Increment
  clicks++;
  if(ms > 800){
    clicks = 0;
  }

  // Print new number
  _scene_counter_write_count();
}

void _scene_counter_stage(){
  _scene_counter_write_count();
}

void _scene_counter_unstage(){
  matrix.clear();
}

void _scene_counter_update(){
}

void _scene_counter_write_count(){
  int digits;

  // get number of digits in clicks
  digits = (int)log10((double)clicks+1)+1;

  // strcpy with correct offset
  strcpy(clicks_str, "000000");
  sprintf(clicks_str+6-digits, "%d", clicks);
  Serial.printf("%s\n", clicks_str);

  // write number
  sentence_write(clicks_str, 16, 0);
}

Scene *scene_counter(){
  Scene *counter_scene;

  // initalize scene
  counter_scene = scene_create("APA");
  counter_scene->stage = _scene_counter_stage;
  counter_scene->unstage = _scene_counter_unstage;
  counter_scene->update = _scene_counter_update;
  counter_scene->button_pressed = _scene_counter_button_pressed;
  counter_scene->button_released = _scene_counter_button_released;

  // Create counter buffer
  clicks_str = (char *)calloc(7, sizeof(char));
  
  return counter_scene;
}
