#include "clockface.h"
#include "scene.h"

static ClockFace *cf = 0x0;

void _scene_clock_button_pressed(){
}

void _scene_clock_button_released(int ms){
  Scene *s;

  if(ms > 700){
    s = scene_find("Menu");
    scene_switch(s);
  }
}

void _scene_clock_stage(){
  clockface_write_now(cf);
}

void _scene_clock_unstage(){
  clockface_erase(cf);
}

void _scene_clock_update(){
  static unsigned long prev_seconds = 0;
  unsigned long epoch_seconds;
  epoch_seconds =  timeClient.getEpochTime();
  if (epoch_seconds != prev_seconds){
    clockface_write_now(cf);
  }
  prev_seconds = epoch_seconds;
}

Scene *scene_clock(){
  Scene *clock_scene;

  // initalize scene
  clock_scene = scene_create("Clock");
  clock_scene->stage = _scene_clock_stage;
  clock_scene->unstage = _scene_clock_unstage;
  clock_scene->update = _scene_clock_update;
  clock_scene->button_pressed = _scene_clock_button_pressed;
  clock_scene->button_released = _scene_clock_button_released;

  // initalize clockface
  cf = clockface_create(0,0, CLOCK_BASE | CLOCK_SECONDS | CLOCK_MILITARY);

  return clock_scene;
}
