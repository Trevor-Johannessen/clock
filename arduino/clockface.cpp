#include "clockface.h"

extern NTPClient timeClient;

ClockFace *clockface_create(int x, int y, unsigned char settings_vector){
  ClockFace *cf;

  // Initalize clockface
  cf = (ClockFace *)malloc(sizeof(*cf));
  cf->x = x;
  cf->y = y;
  cf->prev_time_string = 0x0;
  cf->settings.vector = settings_vector;

  return cf;
}

void clockface_erase(ClockFace *cf){
  clockface_erase_n(cf, 0);
}

void clockface_erase_n(ClockFace *cf, int n){
  int i, width;

  if(cf->prev_time_string){
    width=0;
    for(i=0;i<n;i++)
      width+=character_width(cf->prev_time_string[i])+2;
    sentence_write(cf->prev_time_string+n, cf->x+width, cf->y);
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
  int i, j, length;

  time_string = (char *)calloc(11, sizeof(char));
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
  snprintf(time_string, 11, "%s:%s:%s%s", h, m, s, suffix);

  // Set military
  if(cf->settings.bits.military){
    time_string[8] = '\0';
  }

  // Remove seconds if not set 
  if(!cf->settings.bits.seconds)
    memcpy(time_string+4,time_string+8, 3);

  // Write new time
  if(!cf->prev_time_string)
    sentence_write(time_string, cf->x, cf->y);
  else if(strcmp(time_string, cf->prev_time_string)){
    length = strlen(time_string) > strlen(cf->prev_time_string) ? strlen(cf->prev_time_string) : strlen(time_string);
    for(i=0;i<length && time_string[i] == cf->prev_time_string[i];i++);
    clockface_erase_n(cf, i);
    length = 0;
    for(j=0;j<i;j++)
      length += character_width(time_string[j])+2;
    sentence_write(time_string+i, cf->x+length, cf->y);
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