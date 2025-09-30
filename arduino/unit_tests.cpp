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
  ScheduledTask *task_second, *task_minute_one, *task_minute_five, *task_hour_one, *task_hour_two, *task_hour_less, *task_hour_greater, *task_month_tuesday, *task_month_one,
                *task_month_two, *task_month_two_fifthteen, *task_month_one_fifthteen, *task_day_one, *task_day_two, *task_day_before;
  Date *date;
  char delta;
  long goal;

  date = scheduler_current_date();
  Serial.println("Starting task create");
  task_second = scheduled_task_create(0x84210842108421ULL, 0, 0, 0, 0, 0, 0x0, 0x0); // Every 5 seconds in binary
  task_minute_one = scheduled_task_create(0, 0xFFFFFFFFFFFFFFFFULL, 0, 0, 0, 0, 0x0, 0x0);
  task_minute_five = scheduled_task_create(0, 0x84210842108421ULL, 0, 0, 0, 0, 0x0, 0x0);
  task_hour_one = scheduled_task_create(0, 0, 0xFFFFFFULL, 0, 0, 0, 0x0, 0x0);
  task_hour_two = scheduled_task_create(0, 0, 0xAAAAAAULL, 0, 0, 0, 0x0, 0x0);
  task_hour_less = scheduled_task_create(0, 0, 0xFFFFFFULL, 0, 0, 0, 0x0, 0x0);
  task_hour_greater = scheduled_task_create(0, 0, 0xFFFFFFULL, 0, 0, 0, 0x0, 0x0);
  task_day_one = scheduled_task_create(0, 0, 0, 1<<(date->day%days_in_month[date->month-1]), 0, 0, 0x0, 0x0);
  task_day_two = scheduled_task_create(0, 0, 0, 1<<((date->day+1)%days_in_month[date->month-1]), 0, 0, 0x0, 0x0);
  task_day_before = scheduled_task_create(0, 0, 0, 1<<(date->day-2), 0, 0x200, 0x0, 0x0);
  task_month_tuesday = scheduled_task_create(0, 0, 0, 0, 2, 0x00000200, 0x0, 0x0);
  task_month_one = scheduled_task_create(0, 0, 0, 0, 0, 0x00000200, 0x0, 0x0); // october
  task_month_two = scheduled_task_create(0, 0, 0, 0, 0, 0x00000400, 0x0, 0x0); // november
  task_month_one_fifthteen = scheduled_task_create(0, 0, 0, 0x4000, 0, 1<<(date->month), 0x0, 0x0); // november
  task_month_two_fifthteen = scheduled_task_create(0, 0, 0, 0x4000, 0, 1<<(date->month+1), 0x0, 0x0); // november

  // SCHEDULER_DELTA_VECTOR

  // Target is left of the cursor
  test_run_long("test_scheduler_delta_vector_left", 10, scheduler_delta_vector(24, 1<<19, 9));
  // Target is right of the cursor
  test_run_long("test_scheduler_delta_vector_right", 3, scheduler_delta_vector(5, 0x2, 3));
  // Targets on both sides of the cursor, it should go left
  test_run_long("test_scheduler_delta_vector_sandwich", 10, scheduler_delta_vector(24, (1<<19) + 1, 9));
  // No targets
  test_run_long("test_scheduler_delta_vector_no_target", 255, scheduler_delta_vector(24, 0, 2));


  // SCHEDULER_NEAREST_BIT

  // Checks which bit is lowest
  test_run_long("test_scheduler_nearest_bit", 5,  scheduler_nearest_bit(1<<5));


  // SCHEDULER_CURRENT_DATE (eyeball this one)

  // Prints current date
  Serial.printf("scheduler_current_date: (%d-%d-%d %d:%d:%d [%d])\n", date->year, (int)date->month, (int)date->day, (int)date->hour, (int)date->minute, (int)date->second, (int)date->weekday);

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
  test_run_long("test_scheduler_delta_day_one", seconds_in_day-date->hour*60*60-date->minute*60-date->second, scheduler_delta(task_day_one));
  goal = seconds_in_day*2-date->hour*60*60-date->minute*60-date->second;
  test_run_long("test_scheduler_delta_day_two", goal, scheduler_delta(task_day_two));
  goal = (seconds_in_day-date->hour*60*60-date->minute*60-date->second) + seconds_in_day*(days_in_month[date->month-1]-date->day)+seconds_in_day*(date->day-2);
  test_run_long("test_scheduler_delta_day_before", goal, scheduler_delta(task_day_before));
  goal = (seconds_in_day-date->hour*60*60-date->minute*60-date->second) + seconds_in_day*(days_in_month[date->month-1]-date->day);
  test_run_long("test_scheduler_delta_month_one", goal, scheduler_delta(task_month_one));
  goal = (seconds_in_day-date->hour*60*60-date->minute*60-date->second) + seconds_in_day*(days_in_month[date->month-1]-date->day) + days_in_month[date->month]*seconds_in_day;
  test_run_long("test_scheduler_delta_month_two", goal, scheduler_delta(task_month_two));
  goal = (seconds_in_day-date->hour*60*60-date->minute*60-date->second) + seconds_in_day*(days_in_month[date->month-1]-date->day) + 14*seconds_in_day;
  test_run_long("test_scheduler_delta_month_one_fifthteen", goal, scheduler_delta(task_month_one_fifthteen));
  goal = (seconds_in_day-date->hour*60*60-date->minute*60-date->second) + seconds_in_day*(days_in_month[date->month-1]-date->day) + (days_in_month[date->month]+14)*seconds_in_day;
  test_run_long("test_scheduler_delta_month_two_fifthteen", goal, scheduler_delta(task_month_two_fifthteen));
  //Serial.printf("First tuesday of next month: %d\n",scheduler_delta(task_month_tuesday));

  // Clean up test tasks
  if (task_second) scheduled_task_free(task_second);
  if (task_minute_one) scheduled_task_free(task_minute_one);
  if (task_minute_five) scheduled_task_free(task_minute_five);
  
  if (date) date_free(date);
  // Final memory check
  Serial.printf("Final free heap: %d bytes\n", ESP.getFreeHeap());
}