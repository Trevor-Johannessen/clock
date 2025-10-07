#include "scheduler.h"

unsigned char scheduler_nearest_bit(long long vector){
  int i;
  for(i=0;i<sizeof(vector)*8;i++)
    if((1<<i) & vector)
      return i;
  return 255;
}

void date_free(Date *date){
  free(date);
}

Date *scheduler_current_date(){
  Date *today;
  int i, leap_year_sets;
  unsigned long long now;

  // Alloc date;
  today = (Date *)malloc(sizeof(Date));
  if (!today) {
    Serial.println("ERROR: Failed to allocate memory for Date");
    return NULL;
  }
  today->year=1972;
  today->month=1;
  today->day=1;
  today->weekday = timeClient.getDay();

  // Take current timestamp
  now = timeClient.getEpochTime();

  // Subtract 2 years to base it from 1972 (leap year)
  now -= seconds_in_year*2;

  // divide by number of seconds in leap year cycle (4 years + one day)
  leap_year_sets = now/(seconds_in_year*4+seconds_in_day);
  today->year += leap_year_sets * 4;
  now -= leap_year_sets * (seconds_in_year * 4 + seconds_in_day);

  // iterate subtracting years to see how far into the current leap year cycle we are (after this we will have the current year)
  if (now > seconds_in_year + seconds_in_day){ // first year will be leap year
    now -= seconds_in_year + seconds_in_day;
    today->year+=1;
  }
  while (now > seconds_in_year){
      now -= seconds_in_year;
      today->year+=1;
  }

  // iterate subtracting seconds in each month to find the current month
  for (i=0; i<12; i++){
      if (now > days_in_month[i]*seconds_in_day){
        now -= days_in_month[i]*seconds_in_day;
        if(!(today->year%4) && i == 1) // feb 29.
          now-=1*seconds_in_day;
      }
      else
          break;
      today->month += 1;
  }

  // divide by seconds in a day to find the current day
  today->day = now/seconds_in_day+1;
  now-=seconds_in_day*(today->day-1);

  // Get time
  today->hour = now/60/60;
  now-=60*60*today->hour;
  today->minute = now/60;
  now-=today->minute*60;
  today->second = now;

  return today;
}

int scheduler_date_to_seconds(Date date){
  int seconds;

  seconds = date.year*seconds_in_year;
  seconds += date.day*seconds_in_day;
  seconds += date.hour*60*60;
  seconds += date.minute*60; 
  seconds += date.second;

  return seconds;
}

void scheduler_print_bit_vector(char size, long long vector){
  int i;
  for(i=0;i<size;i++){
    Serial.printf("%d", (1<<i & vector) > 0);
    if((i%8)==7)
      Serial.printf(" "); 
  }
  Serial.printf("\n");
}

unsigned char scheduler_delta_vector(char vector_size, long long vector, char start_pos){
  char i;
  char cur;

  for(i=0;i<vector_size;i++){
    //Serial.printf("i=%d, start_pos+i = %d, vector&1<<i = %d\n", i, start_pos+i,vector & (1LL << cur));
    cur = (start_pos + i) % vector_size;
    if(vector & (1LL << cur))
      return i;
  }
  return 255;
}

unsigned int scheduler_delta(ScheduledTask *task){
  Date *today;
  char delta_hour, delta_second, delta_minute, delta_day, delta_month;
  char start_pos, i;
  long long seconds;

  delta_second = delta_minute = delta_hour = delta_day = delta_month = 0;
  today = scheduler_current_date();
  if (!today) {
    Serial.println("ERROR: Failed to get current date");
    return 0;
  }

  //Serial.printf("Task = %llu s, %llu m, %u h, %u D, %u M\n", task->second, task->minute, task->hour, task->day, task->month);

  // Get Seconds
  delta_second = scheduler_delta_vector(60, task->second, today->second == 59 ? 0 : today->second+1);
  if(delta_second == 255){
    delta_second=0;
    if(task->minute || task->hour || task->day || task->month)
      delta_second = 60 - today->second;
  }
  else
    delta_second++;

  // Get Minutes
  start_pos = today->minute;
  if(60 <= delta_second + today->second)
    start_pos = today->minute >= 59 ? 0 : today->minute+1;
  delta_minute = scheduler_delta_vector(60, task->minute, start_pos);
  if(delta_minute == 255){
    delta_minute=0;
    if(task->hour || task->day || task->month)
      delta_minute = 60 - today->minute-1;
  }

  // Get Hours
  start_pos = today->hour;
  if(60*60 <= (delta_minute + today->minute)*60 + (delta_second + today->second))
    start_pos = today->hour >= 23 ? 0 : today->hour+1;
  delta_hour = scheduler_delta_vector(24, task->hour, start_pos);
  if(delta_hour == 255){
    delta_hour=0;
    if(task->day || task->month)
      delta_hour = 24-today->hour-1;
  }

  // Get Days
  start_pos = today->day-1;
  if(seconds_in_day <= (delta_hour + today->hour)*60*60 + (delta_minute + today->minute)*60 + (delta_second + today->second))
    start_pos = today->day >= days_in_month[today->month-1] ? 0 : today->day;
  // Serial.printf("Day start pos = %d\n", start_pos);
  delta_day = scheduler_delta_vector(days_in_month[today->month-1], task->day, start_pos);
  if(delta_day == 255){
    delta_day=0;
    if(task->month)
      delta_day = days_in_month[today->month-1]-today->day;
  }

  // Get Months
  start_pos = today->month-1;
  if(days_in_month[today->month-1]*seconds_in_day <= (delta_day + today->day-1)*seconds_in_day + (delta_hour + today->hour)*60*60 + (delta_minute + today->minute)*60 + (delta_second + today->second))
    start_pos = today->month >= 11 ? 0 : today->month;
  delta_month = scheduler_delta_vector(12, task->month, start_pos);
  if(delta_month == 255)
    delta_month = 0;

  // Serial.printf("Delta months = %d, Delta days  = %d / %d, Delta hours = %d, Delta minutes = %d, Delta seconds = %d\n", delta_month, delta_day, days_in_month[today->month-1], delta_hour, delta_minute, delta_second);


  // Calculate and return result
  seconds = 0;
  i=0;
  if(!task->day || today->day > task->day){
    i=1;
    delta_month++;
  }
  // if(today->day > task->day && task->day > 1)
  //   delta_month = delta_month < 0 ? delta_month-1 : 0;
  // Serial.printf("i = %d, delta_month = %d\n", i, delta_month);
  for(;i<delta_month;i++){
  // for(;i<delta_month+1;i++){
    // Serial.printf("Adding %d days\n", days_in_month[(today->month-1+i) % 12]);
    seconds+=days_in_month[(today->month-1+i) % 12]*seconds_in_day;
  }
  seconds+=delta_day*seconds_in_day;
  seconds+=delta_hour*60*60;
  seconds+=delta_minute*60;
  seconds+=delta_second;
  
  // Free resources
  date_free(today);
  
  return seconds;
}

ScheduledTaskQueue *scheduler_create(){
  ScheduledTaskQueue *sched;

  sched = (ScheduledTaskQueue *)malloc(sizeof(ScheduledTaskQueue));
  if (!sched) {
    Serial.println("ERROR: Failed to allocate memory for ScheduledTaskQueue");
    return NULL;
  }
  sched->task_list = 0x0;

  return sched;
}

ScheduledTask *scheduler_get_next(ScheduledTaskQueue *queue){
  if(!queue)
    return 0x0;
  return (ScheduledTask *)(queue->task_list->payload);
}

void scheduler_check_task(ScheduledTaskQueue *queue){
  unsigned long long now;
  Link *task_link;
  ScheduledTask *next_task;

  // Check time
  now = timeClient.getEpochTime();
  // If time remove item from linked list
  task_link = queue->task_list;
  next_task = (ScheduledTask *)task_link->payload;
  if(now <= next_task->next_trigger)
    next_task->func(next_task->args);
  // If not a oneoff, reinsert task into list
  queue->task_list = task_link->next;
  link_free(task_link);
  if(!next_task->settings.bits.oneoff)
    scheduler_insert(queue, next_task);
  else
    scheduled_task_free(next_task);
}

void scheduler_insert(ScheduledTaskQueue *queue, ScheduledTask *task){
  Link *cur, *cur_prev;
  ScheduledTask *cur_task;
  unsigned long long now;
  
  if(!queue || !task)
    return;

  now = timeClient.getEpochTime();
  if(!task->next_trigger)
    task->next_trigger = now + scheduler_delta(task);

  if(!queue->task_list){
    queue->task_list = link_create(task);
    return;
  }

  cur=queue->task_list;
  cur_prev=0x0;
  while(cur){
    cur_task = (ScheduledTask *)cur->payload;
    if(cur_task->next_trigger > task->next_trigger){
      // Insert task here
      if(!cur_prev)
        queue->task_list = link_add_first(cur, task);
      else
        link_add_next(cur_prev, task);
      return;
    }
    cur_prev=cur;
    cur=cur->next;
  }
  if(cur_prev)
    link_add_next(cur_prev, task);
}

void scheduler_free(ScheduledTaskQueue *queue){
  Link *current, *next;
  if (queue) {
    current = queue->task_list;
    while (current) {
      next = current->next;
      if (current->payload) {
        scheduled_task_free((ScheduledTask *)current->payload);
      }
      link_free(current);
      current = next;
    }
    free(queue);
  }
}

ScheduledTask *scheduled_task_create(unsigned long long second, unsigned long long minute, unsigned int hour, unsigned int day, unsigned char weekday, unsigned short month, void (*func)(void *), void *args, unsigned char settings){
  ScheduledTask *task;

  task = (ScheduledTask *)malloc(sizeof(ScheduledTask));
  if (!task) {
    Serial.println("ERROR: Failed to allocate memory for ScheduledTask");
    return NULL;
  }
  task->month = month;
  task->day = day;
  task->hour = hour;
  task->minute = minute;
  task->second = second;
  task->weekday = weekday;
  task->next_trigger = 0;
  task->func = func;
  task->args = args;
  task->settings.vector = settings;
   
  return task;
}

void scheduled_task_free(ScheduledTask *task){
  if(task->args)
    free(task->args);
  if (task)
    free(task);
}