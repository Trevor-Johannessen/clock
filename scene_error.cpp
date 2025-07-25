#include "scene.h"
#include "scene_list.h"
#include "letters.h"

#include <MD_MAX72xx.h>

MD_MAX72XX matrix;

void _scene_error_stage(){
  matrix.clear();
  sentence_write("Error");
}

Scene *scene_error(){
  Scene *error_scene;

  // initalize scene
  error_scene = scene_create("error");
  error_scene->stage = _scene_error_stage;
  error_scene->unstage = 0x0;
  error_scene->update = 0x0;
  error_scene->button_pressed = 0x0;
  error_scene->button_held = 0x0;
  error_scene->button_released = 0x0;

  // initalize error
  cf = error_create(0,0, error_BASE);

  return error_scene;
}
