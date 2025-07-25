#include "clockface.h"
#include "scene.h"

static ClockFace *cf = 0x0;

void _scene_clock_button_held(int ms){
}

void _scene_clock_button_pressed(){
}

void _scene_clock_button_released(int interval){
}

void _scene_clock_stage(){
  clockface_write(cf);
}

void _scene_clock_unstage(){
  clockface_erase(cf);
}

void _scene_clock_update(){
  static unsigned long prev_seconds = 0;
  unsigned long epoch_seconds;
  epoch_seconds =  timeClient.getEpochTime();
  if (epoch_seconds != prev_seconds){
    clockface_write(cf);
  }
  prev_seconds = epoch_seconds;
}

Scene *scene_clockface(){
  Scene *clock_scene;

  // initalize scene
  clock_scene = scene_create("clock");
  clock_scene->stage = _scene_clock_stage;
  clock_scene->unstage = _scene_clock_unstage;
  clock_scene->update = _scene_clock_update;
  clock_scene->button_pressed = _scene_clock_button_pressed;
  clock_scene->button_held = _scene_clock_button_held;
  clock_scene->button_released = _scene_clock_button_released;

  // initalize clockface
  cf = clockface_create(0,0, CLOCK_BASE);

  return clock_scene;
}
