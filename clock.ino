#include <MD_MAX72xx.h>
#include <NTPClient.h>
#include <SPI.h>
#include <stdlib.h> // needed for sleep()
#include <time.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "letters.h"
#include "scene.h"
#include "scene_list.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CS_PIN 10
#define BUTTON_PIN 2
#define TIMEZONE -4

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

extern Scene *current_scene;
const int screen_width = 64;
const char *ssid     = "Verizon_RJ93SH";
const char *password = "psyche-han4-jag";
int button_state = LOW;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for Leonardo only
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  // Initalize LED Matrix
  matrix.begin();
  matrix.clear();
  matrix.control(MD_MAX72XX::INTENSITY, 5);  // Set brightness

  // Initalize Time
  timeClient.setTimeOffset(3600*TIMEZONE);
  timeClient.begin();
  timeClient.update();

  // Initalize Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initalize Scenes
  scene_save(scene_menu());
  scene_save(scene_clock());
  scene_save(scene_timer());
  scene_save(scene_counter());

  // Transition to first scene
  scene_switch(scene_clock());
}

void loop() {
  static int prev_button_state = LOW, held_lights_on = 0, held_light_blink = 1, held_light_last_blink;
  static long ms_press_start = 0;
  int ms, button_state, elapsed_time, i;

  button_state = digitalRead(BUTTON_PIN);
  if(button_state == HIGH  && prev_button_state == LOW){ // press
    ms_press_start = millis();
    current_scene->button_pressed();
  }else if(prev_button_state == HIGH && button_state == HIGH){ // hold
    i = held_lights_on;
    held_lights_on = (millis()-ms_press_start) / 100;
    if(held_lights_on < 14){ // building held lights
      if(held_lights_on > 7)
        held_lights_on = 7;
      for(;i<held_lights_on+1; i++)
        matrix.setPoint(i, screen_width-1, 1);
    }else if(millis()-held_light_last_blink > 300){ // full, start blinking
      held_light_blink*=-1;
      held_light_last_blink = millis();
      for(i=0;i<8; i++)
        matrix.setPoint(i, screen_width-1, held_light_blink>0);
    }
  }else if(prev_button_state == HIGH && button_state == LOW){ // release
    // Turn off held lights
    for(i=0; i<held_lights_on+1;i++)
      matrix.setPoint(i, screen_width-1, 0);
    held_lights_on = 0;
    // Update Scene
    elapsed_time = millis() - ms_press_start;
    Serial.printf("Held button for %ldms.\n", elapsed_time);
    if(elapsed_time > 2000){
      scene_switch(scene_find("Menu"));
      goto update_cleanup;
    }else
      current_scene->button_released(elapsed_time);
  }
  current_scene->update();
  update_cleanup:
  prev_button_state = button_state;
}
