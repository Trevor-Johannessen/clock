#include "clockface.h"

extern NTPClient timeClient;

ClockFace *clockface_create(int x, int y, unsigned char settings_vector){
  ClockFace *cf;
  
  cf = (ClockFace *)malloc(sizeof(*cf));
  cf->x = x;
  cf->y = y;
  cf->prev_time_string = 0x0;
  cf->settings.vector = settings_vector;

  return cf;
}

void clockface_erase(ClockFace *cf){
  if(cf->prev_time_string){
    sentence_write(cf->prev_time_string, cf->x, cf->y);
    free(cf->prev_time_string);
    cf->prev_time_string = 0x0;
  }
}

void clockface_free(ClockFace *cf){
  if(cf->prev_time_string)
    free(cf->prev_time_string);
  free(cf);
}

void clockface_write(ClockFace *cf, int hour, int minute, int second){
  char *time_string, *suffix;
  char h[3],m[3],s[3];

  time_string = (char *)calloc(13, sizeof(char));
  suffix = (char *)malloc(3*sizeof(char));
  suffix[1] = 'M';

  // if AM/PM set overhead
  if(!cf->settings.bits.military){
    if(hour < 12)
      suffix[0] = 'A';
    else {
      suffix[0] = 'P';
      hour-=12;
    }
    if(hour == 0)
      hour = 12;
  }

  // Get time
  snprintf(h, 3, "%02d", hour);
  snprintf(m, 3, "%02d", minute);
  snprintf(s, 3, "%02d", second);

  // Set full time (assume military with seconds)
  snprintf(time_string, 13, "%s::%s::%s%s", h, m, s, suffix);

  // Set military
  if(cf->settings.bits.military){
    time_string[10] = '\0';
  }

  // Remove seconds if not set 
  if(!cf->settings.bits.seconds)
    memcpy(time_string+6,time_string+10, 3);

  // Write new time
  if(!cf->prev_time_string)
    sentence_write(time_string, cf->x, cf->y);
  else if(strcmp(time_string, cf->prev_time_string)){
    clockface_erase(cf);
    sentence_write(time_string, cf->x, cf->y);
  }

  // Cleanup
  if(cf->prev_time_string)
    free(cf->prev_time_string);
  free(suffix);
  cf->prev_time_string = time_string;
}

void clockface_write_now(ClockFace *cf){
  int hour, minute, second;
  
  hour = timeClient.getHours();
  minute = timeClient.getMinutes();
  second = timeClient.getSeconds();
  clockface_write(cf, hour, minute, second);
}