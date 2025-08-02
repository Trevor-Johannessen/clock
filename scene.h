#ifndef SCENE_H
#define SCENE_H

#include "linked-list.h" 

typedef struct {

  void (*stage)();
  void (*unstage)();
  void (*update)();
  void (*button_pressed)();
  void (*button_released)(int);
  char *name;
} Scene;

extern Link *saved_scenes;
extern Scene *current_scene;

Scene *scene_create(char *name);
Scene *scene_find(char *name);
void scene_free();
void scene_save(Scene *scene);
Scene *scene_switch(Scene *new_scene);

#endif
