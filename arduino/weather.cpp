#include "letters.h"
#include "weather.h"

const char *api_key = "";
const char *weather_url  = "";

Weather *weather_create(char *location_key, unsigned char settings){
  Weather *weather;
  char *key;

  weather = (Weather *)malloc(sizeof(weather));
  key = (char *)malloc(strlen(location_key));

  strcpy(key, location_key);
  weather->settings.vector = settings;
  weather->location_key = key;
  weather->temperature = 0;
  weather->icon = 0;


  return weather;
}

void weather_free(Weather *weather){
  if(!weather)
    return;
  if(weather->location_key)
    free(weather->location_key);
  free(weather);
}

void weather_write(Weather *weather, int x, int y){
  char *output, format[3];
  int output_length, i;

  // Get string length
  output_length = (int)(ceil(log10(int(weather->temperature)))+1);
  output_length += weather->settings.bits.show_deg_symbol ? 1 : 0;
  output_length += weather->settings.bits.show_unit ? 1 : 0;
  output_length += weather->settings.bits.metric ? 2 : 0; // space for decimal point and one decimal place
  output = (char *)calloc(output_length+1, sizeof(char));
  
  // Assemble format
  for(i=0;i<3;i++) format[i]='\0';
  if(weather->settings.bits.show_deg_symbol)format[strlen(format)] = CHARACTER_DEG;
  if(weather->settings.bits.show_unit)format[strlen(format)] = weather->settings.bits.metric ? 'C' : 'F';

  // Write temperature
  if(weather->settings.bits.metric)
    sprintf(output, "%.1lf%s", weather->temperature, format);
  else
    sprintf(output, "%d%s", (int)weather->temperature, format);
  
  // Write text and cleanup
  sentence_write(output, x, y);
  free(output);
}

void weather_update(Weather *weather){
  String body_string;
  char *bearer_token, *body, icon, *str, *temp_string, *request_url;
  int i, request_url_len;

  // Format request url
  request_url_len = strlen(weather_url) + strlen("/weather?location_key=") + strlen(weather->location_key) + strlen("&metric=") + (weather->settings.bits.metric ? 4 : 5) + 1; // 4 for 'true', 5 for 'false'
  request_url = (char *)calloc(request_url_len, sizeof(char));
  snprintf(request_url, request_url_len, "%s/weather?location_key=%s&metric=%s", weather_url, weather->location_key, weather->settings.bits.metric ? "true" : "false");
  http.begin(request_url);

  // Add headers
  bearer_token = (char *)malloc(strlen("Bearer ") + strlen(api_key) + 1);
  sprintf(bearer_token, "Bearer %s", api_key);
  http.addHeader("Authorization", bearer_token);

  // Get temperature and icon
  if(http.GET() != 200){
    Serial.printf("%s/weather endpoint error.\n", weather_url);
    goto cleanup;
  }
  body_string = http.getString();
  body = (char *)malloc(body_string.length() + 1);
  strcpy(body, body_string.c_str());
  for(str=body,i=0; str[i] != ',' && i < strlen(body);i++);
  str[i] = '\0';
  icon = atoi(str);
  weather->temperature = atof(str+i+1);
  Serial.printf("Successfully parsed body. Symbol=%d, Temperature=%f\n", weather->icon, weather->temperature);

  // Set icon
  if(icon < 6) // sunny
    weather->icon = CHARACTER_ICON_SUN;
  else if(icon < 12) // cloudy
    weather->icon = CHARACTER_ICON_CLOUD;
  else if(icon < 14) // rainy
    weather->icon = CHARACTER_ICON_RAIN;
  else if(icon < 18) // thunder
    weather->icon CHARACTER_ICON_STORM;
  else if(icon < 20) // more rain
    weather->icon = CHARACTER_ICON_RAIN;
  else if(icon < 22) // more cloudy
    weather->icon = CHARACTER_ICON_CLOUD;
  else if(icon < 30) // snowy
    weather->icon = CHARACTER_ICON_SNOW;
  else if(icon < 35) // more Sunny
    weather->icon CHARACTER_ICON_SUN;
  else if(icon < 39) // more cloudy
    weather->icon = CHARACTER_ICON_CLOUD;
  else if(icon < 41)
    weather->icon = CHARACTER_ICON_RAIN;
  else if(icon < 43)
    weather->icon = CHARACTER_ICON_STORM;
  else if(icon < 44)
    weather->icon = CHARACTER_ICON_CLOUD;
  else if(icon < 45)
    weather->icon = CHARACTER_ICON_SNOW;
  else
    weather->icon = CHARACTER_ICON_SUN
  
  cleanup:
  http.end();
}