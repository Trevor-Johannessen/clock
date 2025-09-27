#include "unit_tests.h"

void test_run_long(char *name, long a, long b){
  int output;

  output = a == b;
  if(!output){
    Serial.printf("Test %s failed. (expected %ld, got %ld)\n", name, a, b);
    while(1);
  }
  Serial.printf("Test %s passed!\n", name);
}

void test_run_double(char *name, double a, double b){
    int output;

  output = a == b;
  if(!output){
    Serial.printf("Test %s failed. (expected %f, got %f)\n", name, a, b);
    while(1);
  }
  Serial.printf("Test %s passed!\n", name);
}

void test_run_str(char *name, char *a, char *b){ // use static string for answer
  if(strncmp(a, b, strlen(a))){
    Serial.printf("Test %s failed. (expected %s, got %s)\n", name, a, b);
    while(1);
  }
  Serial.printf("Test %s passed!\n", name);
}

void test_scheduler(){
  ScheduledTask *task_second, *task_minute_one, *task_minute_five, *task_hour_one, *task_hour_two, *task_hour_less, *task_hour_greater;
  Date *date;
  char delta;

  Serial.println("Starting task create");
  task_second = scheduled_task_create(0x84210842108421ULL, 0, 0, 0, 0, 0, 0x0, 0x0); // Every 5 seconds in binary
  task_minute_one = scheduled_task_create(0, 0xFFFFFFFFFFFFFFFFULL, 0, 0, 0, 0, 0x0, 0x0);
  task_minute_five = scheduled_task_create(0, 0x84210842108421ULL, 0, 0, 0, 0, 0x0, 0x0);
  task_hour_one = scheduled_task_create(0, 0, 0xFFFFFFULL, 0, 0, 0, 0x0, 0x0);
  task_hour_two = scheduled_task_create(0, 0, 0xAAAAAAULL, 0, 0, 0, 0x0, 0x0);
  task_hour_less = scheduled_task_create(0, 0, 0xFFFFFFULL, 0, 0, 0, 0x0, 0x0);
  task_hour_greater = scheduled_task_create(0, 0, 0xFFFFFFULL, 0, 0, 0, 0x0, 0x0);

  // SCHEDULER_DELTA_VECTOR

  // Target is left of the cursor
  test_run_long("test_scheduler_delta_vector_left", 10, scheduler_delta_vector(24, 1<<19, 9));
  // Target is right of the cursor
  test_run_long("test_scheduler_delta_vector_right", 15, scheduler_delta_vector(24, 1, 9));
  // Targets on both sides of the cursor, it should go left
  test_run_long("test_scheduler_delta_vector_sandwich", 10, scheduler_delta_vector(24, (1<<19) + 1, 9));
  // No targets
  test_run_long("test_scheduler_delta_vector_no_target", 255, scheduler_delta_vector(24, 0, 2));


  // SCHEDULER_NEAREST_BIT

  // Checks which bit is lowest
  test_run_long("test_scheduler_nearest_bit", 5,  scheduler_nearest_bit(1<<5));


  // SCHEDULER_CURRENT_DATE (eyeball this one)

  // Prints current date
  Serial.println("Testing scheduler_current_date.");
  date = scheduler_current_date();
  Serial.printf("scheduler_current_date: (%d-%d-%d %d:%d:%d [%d])\n", date->year, (int)date->month, (int)date->day, (int)date->hour, (int)date->minute, (int)date->second, (int)date->weekday);
  Serial.println("Freeing date");
  date_free(date);

  // SCHEDULER_DELTA
  Serial.println("Testing scheduler_delta.");
  test_run_long("test_scheduler_delta_second_5", timeClient.getEpochTime()%5 ? 5-(timeClient.getEpochTime()%5) : 0, scheduler_delta(task_second));
  test_run_long("test_scheduler_delta_minute_1", 60-date->second, scheduler_delta(task_minute_one));
  test_run_long("test_scheduler_delta_minute_5", 300-((date->minute%5)*60)-date->second, scheduler_delta(task_minute_five));
  test_run_long("test_scheduler_delta_hour_1", 1*60*60-date->minute*60-date->second, scheduler_delta(task_hour_one));
  task_hour_two->hour = 0xAAAAAAULL << !(date->hour%2);
  test_run_long("test_scheduler_delta_hour_2", 2*60*60-date->minute*60-date->second, scheduler_delta(task_hour_two));
  task_hour_less->minute = date->minute < 59 ? 1<<(date->minute+1) : 1;
  test_run_long("test_scheduler_delta_hour_less", 60-date->second, scheduler_delta(task_hour_less));
  task_hour_greater->minute = date->minute < 59 ? 1<<(date->minute+1) : 1;
  test_run_long("test_scheduler_delta_hour_greater", 2*60*60-date->minute*60-date->second, scheduler_delta(task_hour_two));

  // Clean up test tasks
  if (task_second) scheduled_task_free(task_second);
  if (task_minute_one) scheduled_task_free(task_minute_one);
  if (task_minute_five) scheduled_task_free(task_minute_five);
  
  // Final memory check
  Serial.printf("Final free heap: %d bytes\n", ESP.getFreeHeap());
}