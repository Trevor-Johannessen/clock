#include <NTPClient.h>
#include <stdlib.h>
#include "clockface.h"
#include "letters.h"
#include "scene.h"

#define TIMER_UNSET 0
#define TIMER_SET_HOUR 1
#define TIMER_SET_MINUTE 2
#define TIMER_SET_SECOND 3
#define TIMER_ACTIVE 4
#define TIMER_PAUSED 5
#define TIMER_ALERTING 6
#define TIMER_INCREMENT 1
#define TIMER_DECREMENT -1

void _scene_timer_set_hour();
void _scene_timer_write_blink();
void _scene_timer_write_countdown(int erase_only);

extern NTPClient timeClient;

int timer_state = TIMER_SET_HOUR;
long timer_set_time = 0;
long timer_paused_time = 0;
ClockFace *timer_cf;
static long last_timestamp = 0;

void _scene_timer_button_pressed(){
}

void _scene_timer_bump(char *number_string, int *timer_counter, int direction, int max){
  sentence_write(number_string, 0, 0);
  *timer_counter += direction;
  if(*timer_counter > max)
    *timer_counter = 0;
  else if(*timer_counter < 0)
    *timer_counter = max;
  snprintf(number_string, 3, "%02d", *timer_counter);
  sentence_write(number_string, 0, 0);
}

void _scene_timer_handle_select(int ms, char *number_string, int *timer_counter, int *timer_delta, int state){
  int multiplier = 1;
  if(ms > 700){ // move to next state
    if(state != TIMER_SET_SECOND)
      multiplier = state == TIMER_SET_MINUTE ? 60 : 60*60;
    *timer_delta += *timer_counter*multiplier;
    *timer_counter = 0;
    sentence_write(number_string, 0, 0);
    number_string[0] = '0';
    number_string[1] = '0';
    if(state < TIMER_SET_SECOND)
      sentence_write(number_string, 0, 0);
    switch(timer_state){
      case TIMER_SET_HOUR:
        sentence_write("PPP", 32, 0);
        sentence_write("AA", 32, 0);
        break;
      case TIMER_SET_MINUTE:
        sentence_write("AA", 32, 0);
        sentence_write("P", 32, 0);
        break;
      case TIMER_SET_SECOND:
        sentence_write("P", 32, 0);
        break;
    }
    timer_state++;
  }else if(ms > 300)
    _scene_timer_bump(number_string, timer_counter, TIMER_DECREMENT, state == TIMER_SET_HOUR ? 23 : 59);
  else
    _scene_timer_bump(number_string, timer_counter, TIMER_INCREMENT, state == TIMER_SET_HOUR ? 23 : 59);
}

void _scene_timer_button_released(int ms){
    static char number_string[3] = "00";
    static int timer_delta = 0, timer_counter = 0;

    switch(timer_state){
      case TIMER_UNSET:
        break;
      case TIMER_ACTIVE:
        if(ms > 700)
          timer_state = TIMER_SET_HOUR;
        else{
          timer_paused_time = timeClient.getEpochTime();
          timer_state = TIMER_PAUSED;
        }
        break;
      case TIMER_SET_HOUR:
        _scene_timer_handle_select(ms, number_string, &timer_counter, &timer_delta, timer_state);
        break;
      case TIMER_SET_MINUTE:
        _scene_timer_handle_select(ms, number_string, &timer_counter, &timer_delta, timer_state);
        break;
      case TIMER_SET_SECOND:
        _scene_timer_handle_select(ms, number_string, &timer_counter, &timer_delta, timer_state);
        if(timer_state == TIMER_ACTIVE)
          timer_set_time = timeClient.getEpochTime() + timer_delta;
        break;
      case TIMER_PAUSED:
        timer_state = TIMER_ACTIVE;
        timer_set_time += timeClient.getEpochTime() - timer_paused_time;
        break;
      case TIMER_ALERTING:
        number_string[0] = '0';
        number_string[1] = '0';
        timer_delta = 0;
        timer_counter = 0;
        _scene_timer_set_hour();
        break;
   }
}

void _scene_timer_set_hour(){
  matrix.clear();
  timer_cf->prev_time_string = 0x0;
  timer_state = TIMER_SET_HOUR;
  last_timestamp = 0;
  timer_set_time = 0;
  _scene_timer_write_countdown(1);
  sentence_write("00", 0, 0);
  sentence_write("PPP", 32, 0);
}

void _scene_timer_stage(){
  if(timer_state != TIMER_ACTIVE && timer_state != TIMER_PAUSED){
    _scene_timer_set_hour();
  }
}

void _scene_timer_unstage(){
  matrix.clear();
}

void _scene_timer_update(){
  static long last_timestamp = 0;
  static char interval_state = 0;
  uint16_t col;
  switch(timer_state){
    case TIMER_ACTIVE:
      _scene_timer_write_countdown(0);
      if(timer_set_time <= timeClient.getEpochTime())
        timer_state = TIMER_ALERTING;
      break;
    case TIMER_PAUSED:
      // Blink when paused
      if(millis() % 1000 >= 500 && interval_state){
        _scene_timer_write_blink();
        interval_state = 0;
      }else if(millis() % 1000 < 500 && !interval_state){
        clockface_erase(timer_cf);
        interval_state = 1;
      }
      break;
    case TIMER_ALERTING:
      if(millis() % 1000 >= 500 && interval_state){
        for (col = 0; col < matrix.getColumnCount(); col++)
          matrix.setColumn(col, 0xFF);
        sentence_write("AAAAA", 12, 0);
        interval_state = 0;
      }else if(millis() % 1000 < 500 && !interval_state){
        matrix.clear();
        sentence_write("AAAAA", 12, 0);
        interval_state = 1;
      }
      break;
  }
}

void _scene_timer_write_blink(){
  int hour, minute, second;
  long diff;

  diff = timer_set_time - timer_paused_time;
  hour = (diff - diff%60) / 60 / 60;
  minute = (diff - diff%60) / 60 % 60;
  second = diff % 60;
  clockface_write(timer_cf, hour, minute, second);
}

void _scene_timer_write_countdown(int erase_only){
  static int last_hour = 0, last_minute = 0, last_second = 0;
  long now, diff;
  if((now = timeClient.getEpochTime()) == last_timestamp)
    return;
  diff = timer_set_time - now;
  if (last_timestamp){
    clockface_write(timer_cf, last_hour, last_minute, last_second);
  }
  if(erase_only){
    last_timestamp = last_hour = last_minute = last_second = 0;
    return;
  }
  last_hour = (diff - diff%60) / 60 / 60;
  last_minute = (diff - diff%60) / 60 % 60;
  last_second = diff % 60;
  last_timestamp = now;
  clockface_write(timer_cf, last_hour, last_minute, last_second);
}

Scene *scene_timer(){
  Scene *menu_scene;

  // initalize scene
  menu_scene = scene_create("PPP");
  menu_scene->stage = _scene_timer_stage;
  menu_scene->unstage = _scene_timer_unstage;
  menu_scene->update = _scene_timer_update;
  menu_scene->button_pressed = _scene_timer_button_pressed;
  menu_scene->button_released = _scene_timer_button_released;

  // initalize clockface
  timer_cf = clockface_create(0,0, CLOCK_SECONDS | CLOCK_MILITARY);

  return menu_scene;
}
