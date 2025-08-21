#ifndef CLOCKFACE_H
#define CLOCKFACE_H

#include "letters.h"
#include <stdlib.h>
#include <time.h>
#include <NTPClient.h>

#define CLOCK_BASE 0x0
#define CLOCK_SECONDS 0x1
#define CLOCK_BLINK 0x2
#define CLOCK_MILITARY 0x4

extern NTPClient timeClient;

typedef struct {
  unsigned char seconds: 1;
  unsigned char blink : 1;
  unsigned char military : 1;
} ClockFaceSettings;

typedef struct {
  union {
    ClockFaceSettings bits;
    char vector;
  } settings;
  char *prev_time_string;
  int x;
  int y;
} ClockFace;

ClockFace *clockface_create(int x, int y, unsigned char settings_vector);
void clockface_erase(ClockFace *cf);
void clockface_free(ClockFace *cf);
void clockface_write(ClockFace *cf, int hour, int minute, int second);
void clockface_write_now(ClockFace *cf);

#endif