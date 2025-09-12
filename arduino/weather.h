#ifndef WEATHER_H
#define WEATHER_H

#include <HTTPClient.h>

#define WEATHER_BASE 0x0
#define WEATHER_METRIC 0x1
#define WEATHER_ICON 0x2
#define WEATHER_DEG_SYMBOL 0x4
#define WEATHER_UNIT 0x8

#define CHARACTER_ICON_SUN 4
#define CHARACTER_ICON_CLOUD 5
#define CHARACTER_ICON_RAIN 6
#define CHARACTER_ICON_STORM 7
#define CHARACTER_ICON_SNOW 8

typedef struct {
  unsigned char metric: 1;
  unsigned char show_icon : 1;
  unsigned char show_deg_symbol : 1; // controls the little o symbol
  unsigned char show_unit : 1; // Writes C or F after the temperature
} WeatherSettings;

typedef struct {
  unsigned char icon;
  union {
    WeatherSettings bits;
    unsigned char vector;
  } settings;
  double temperature;
  char *location_key;
} Weather;

extern HTTPClient http;
extern const char *api_key;
extern const char *weather_url;

Weather *weather_create(char *location_key, unsigned char settings);
void weather_free(Weather *weather);
void weather_write(Weather *weather, int x, int y);
void weather_update(Weather *weather);


#endif
