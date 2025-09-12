#include "scheduler.h"

unsigned char scheduler_nearest_bit(long vector){
  int i;
  for(i=0;i<sizeof(vector)*8;i++)
    if((1<<i) & vector)
      return i;
  return 255;
}

Date *scheduler_current_date(){
  Date *today;
  int i, leap_year_sets;
  unsigned long now;

  // Alloc date;
  today = (Date *)malloc(sizeof(today));
  today->year=1972;
  today->month=1;
  today->day=1;
  today->weekday = timeClient.getDay();

  // Take current timestamp
  now = timeClient.getEpochTime();

  // Subtract 2 years to base it from 1972 (leap year)
  now -= 31536000*2;

  // divide by number of seconds in leap year cycle (4 years + one day)
  leap_year_sets = now/(31536000*4+seconds_in_day);
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

unsigned char scheduler_delta_vector(char vector_size, long vector, long start_pos){
  char i;
  long cur;

  for(i=0,cur=start_pos;i<vector_size;i++,cur++){
    if(cur >= vector_size)
      cur=0;
    if(vector & (1<<cur))
      return i;
  }
  return 255;
}

unsigned long scheduler_delta(ScheduledTask *task){
  Date *today;
  unsigned char month_delta, month, days_weekday, days_month, delta_hour, delta_second, delta_minute, delta_day;
  unsigned int this_month_vector, interm_months;
  int i, j, task_seconds, today_seconds, weekdays;
  long delta;

  delta = 0;
  today = scheduler_current_date();

  // Calculate what time of day cron will fire
  delta_hour = scheduler_delta_vector(24, task->hour, today->hour);
  delta_hour = delta_hour == 255 ? 0 : delta_hour;
  delta_minute = scheduler_delta_vector(60, task->minute, today->minute);
  delta_minute = delta_minute == 255 ? 0 : delta_minute;
  delta_second = scheduler_delta_vector(60, task->second, today->second);
  delta_second = delta_second == 255 ? 0 : delta_second;
  task_seconds = delta_hour*60*60 + delta_minute*60 + delta_second;
  today_seconds = today->hour*60*60 + today->minute*60 + today->second;
  Serial.printf("Delta hour: %d\nDelta minute: %d\nDelta second: %d\nToday's time: %d");

  // Calculate weekdays
  weekdays = scheduler_delta_vector(7, task->weekday, today->weekday);
  if(weekdays == 255) // If no weekday set, automatically go with day of month (>365)
    weekdays=999;

  // If cron will happen later today
  if ((task->month & (1<<(today->day-1))) && task_seconds > today_seconds){
    delta_day=0;
    delta=task_seconds - today_seconds;
  } else {
    this_month_vector = task->month << (32-days_in_month[month]);
    this_month_vector = this_month_vector >> (32-days_in_month[month]);
    delta_day = scheduler_nearest_bit(this_month_vector>>today->day); // If the cron has not happened this month
    if (delta_day == 255){ // If the cron has already happened yet this month
        delta_day=(days_in_month[today->month] - today->day-1) + scheduler_nearest_bit(task->month); // add days in next month
        interm_months = scheduler_delta_vector(task->month, 12, today->month)-1;
        for(i=0,j=today->month+1; i<interm_months; i++){
          delta_day = seconds_in_day * days_in_month[j]; // add interm months
          j++;
          if(j>11);
            j=0;
        }
    }
    delta = (delta_day > weekdays ? delta_day : weekdays) * seconds_in_day;
    delta += (23-today->hour)*60*60 + (59-today->minute)*60 + (59-today->second); // add seconds of current day
    // add seconds on active day
    delta += task->hour ? scheduler_nearest_bit(task->hour)*60*60 : 0;
    delta += task->minute ? scheduler_nearest_bit(task->minute)*60 : 0; 
    delta += task->second; 
  }

  return delta;
}

ScheduledTaskQueue *scheduler_create(){
  ScheduledTaskQueue *sched;

  sched = (ScheduledTaskQueue *)malloc(sizeof(sched));
  sched->task_list = 0x0;
  sched->next_task_seconds = 0;

  return sched;
}

ScheduledTask *scheduler_get_next(ScheduledTaskQueue *queue){
  if(!queue)
    return 0x0;
  return (ScheduledTask *)(queue->task_list->payload);
}

void scheduler_check_task(ScheduledTaskQueue *queue){
  // Check time

  // If time remove item from linked list

  // If not a oneoff, reinsert task into list
}

void scheduler_insert(ScheduledTaskQueue *queue, ScheduledTask *task){

}

void scheduler_free(ScheduledTaskQueue *queue){

}

ScheduledTask *scheduled_task_create(unsigned long second, unsigned long minute, unsigned int hour, unsigned int day, unsigned char weekday, unsigned short month, void (*func)(void *), void *args){
  ScheduledTask *task;

  task = (ScheduledTask *)malloc(sizeof(task));
  task->month = month;
  task->day = day;
  task->hour = hour;
  task->minute = minute;
  task->second = second;
  task->weekday = weekday;
  task->func = func;
  task->args = args;
   
  return task;
}

void scheduled_task_free(ScheduledTask *task){

}