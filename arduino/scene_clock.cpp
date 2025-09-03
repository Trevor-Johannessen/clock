#include "clockface.h"
#include "scene.h"
#include "weather.h"

static ClockFace *cf = 0x0;
static Weather *weather = 0x0;

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
  if(weather->icon)
    weather_write(weather, 48, 0);
  Serial.println("Finished staging");
}

void _scene_clock_unstage(){
  clockface_erase(cf);
  if(weather->icon)
    weather_write(weather, 48, 0);
}

void _scene_clock_update(){
  static unsigned long prev_seconds = 0, last_weather_fetch = 0;
  static char last_weather = 0; // tracks what the last whether was shown, 'T' for Temperature 'I' for Icon
  unsigned long epoch_seconds;
  double old_temperature, new_temperature;

  epoch_seconds = timeClient.getEpochTime();
  // Update time
  if (epoch_seconds != prev_seconds){
    clockface_write_now(cf);
  }
  prev_seconds = epoch_seconds;

  // Update weather
  if (epoch_seconds % 1000 < 500){ // Show temperature
    if (last_weather == 'I'){
      character_write(weather->icon, 48, 0);
      last_weather = 'T';
    }
    if (epoch_seconds - last_weather_fetch > 900){ // Get weather every 15 minutes (900 seconds)
      old_temperature = weather->temperature;
      weather_update(weather);
      new_temperature = weather->temperature;
      if(weather->icon && old_temperature != new_temperature){
        weather->temperature = old_temperature;
        weather_write(weather, 48, 0);
        weather->temperature = new_temperature;
        weather_write(weather, 48, 0);
        Serial.printf("Weathers written\n");
      }
      last_weather_fetch = epoch_seconds;
    }
  }else{ // Show icon
    if (last_weather == 'T'){
      weather_write(weather, 48, 0);
      character_write(weather->icon, 48, 0);
      last_weather = 'I';
    }
  }
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

  // initalize objects
  cf = clockface_create(2,0, CLOCK_BASE | CLOCK_SECONDS | CLOCK_MILITARY);
  weather = weather_create("329592", WEATHER_BASE | WEATHER_DEG_SYMBOL);
  weather_update(weather);

  return clock_scene;
}
