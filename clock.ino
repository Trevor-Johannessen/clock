#include "clockface.h"
#include "letters.h"
#include "scene.h"
#include "scene_list.h"
#include <MD_MAX72xx.h>
#include <NTPClient.h>
#include <SPI.h>
#include <stdlib.h> // needed for sleep()
#include <time.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CS_PIN 10
#define BUTTON_PIN 3
#define TIMEZONE -4

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

extern Scene *current_scene;
const int screen_width = 64;
int button_state = LOW;

int elapsed_milliseconds(){
  static int prev_ms = 0;
  int curr_ms;
  curr_ms = millis();
  if(curr_ms > prev_ms)
    return curr_ms-prev_ms;
  return 1+curr_ms-prev_ms;
}

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

  // Initalize Scenes
  current_scene = scene_clockface();
  current_scene->stage();
}

void loop() {
  const int interval = 200;
  static int held_ms = 0, last_held_update_ms = 0, prev_button_state = LOW;
  int ms, button_state; 

  button_state = digitalRead(BUTTON_PIN);
  // if button held add ms to held ms
  if(button_state == HIGH){
    ms += elapsed_milliseconds();
    held_ms += ms;
    last_held_update_ms+=ms;
    if(prev_button_state == LOW){ // press
      Serial.println("Button Pressed!");
      current_scene->button_pressed();
    } else if(last_held_update_ms > 50){ // update every 50ms
      current_scene->button_held(held_ms);
      last_held_update_ms = 0;
    }
  }
  if(prev_button_state == HIGH &&button_state == LOW){ // release
    current_scene->button_released(held_ms/200);
    held_ms = 0;
  }
  // on release set held_ms back to 0
  current_scene->update();
  prev_button_state = button_state;
}
