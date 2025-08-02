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

  // Initalize Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initalize Scenes
  scene_switch(scene_clock());
}

void loop() {
  static int ms_press_start = 0, prev_button_state = LOW;
  int ms, button_state;

  button_state = digitalRead(BUTTON_PIN);
  if(button_state == HIGH  && prev_button_state == LOW){ // press
    Serial.printf("Button Pressed at %d seconds!\n", millis()/1000);
    ms_press_start = millis();
    current_scene->button_pressed();
  }
  if(prev_button_state == HIGH && button_state == LOW){ // release
    Serial.printf("Held button for %d seconds\n", (millis() - ms_press_start)/1000);
    current_scene->button_released(millis() - ms_press_start);
  }
  prev_button_state = button_state;
  current_scene->update();
}
