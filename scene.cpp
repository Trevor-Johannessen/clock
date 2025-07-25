#include "scene.h"

Link *saved_scenes = 0x0;
Scene *current_scene = 0x0;

Scene *scene_create(char *name){
  Scene *scene;
  char *name_buf;

  // Copy name
  name_buf = (char *)malloc(strlen(name)+1);
  strcpy(name_buf, name);

  // Create scene
  scene = (Scene *)malloc(sizeof(Scene));
  scene->name = name_buf;

  return scene;
}

Scene *scene_find(char *name){
  Link *cur;

  for(cur=saved_scenes; link_has_next(cur); cur=link_next(cur))
      if(!strcmp(((Scene *)cur->payload)->name, name))
        return (Scene *)cur->payload;
  return 0x0;
}

void scene_free(Scene *scene){
  if(!scene)
    return;
  if(scene->name)
   free(scene->name);
  free(scene);
}

void scene_save(Scene *scene){
  if(link_contains(saved_scenes, scene))
    return;
  link_add_next(saved_scenes, scene);
}

Scene *scene_switch(Scene *new_scene, struct timeval *tv){
  Scene *old_scene;

  if(current_scene)
    current_scene->unstage();
  old_scene = current_scene;
  if(new_scene)
    new_scene->stage();
  current_scene = new_scene;

  return old_scene;
}
