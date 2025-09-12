#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <NTPClient.h>
#include "linked-list.h"

typedef struct {
  Link *task_list;
  int next_task_seconds;
} ScheduledTaskQueue;

typedef struct {
  int year;
  char month;
  char day;
  char hour;
  char minute;
  char second;
  char weekday;
} Date;

/*
  For each value in the cron flip the corresponding bits in the number
  Example:
    December -> month |= 1<<(12-1)
*/
typedef struct {
  unsigned long second;
  unsigned long minute;
  unsigned int hour;
  unsigned int day;
  unsigned char weekday;
  unsigned short month;
  void (*func)(void *);
  void *args;
} ScheduledTask;

extern NTPClient timeClient;
const int seconds_in_day = 86400;
const long seconds_in_year = 31556926;
const char days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

Date *scheduler_current_date();
unsigned long scheduler_delta(ScheduledTask *task);
unsigned char scheduler_nearest_bit(long vector);
void scheduler_check_task(ScheduledTaskQueue *queue);
unsigned char scheduler_delta_vector(char vector_size, long vector, long start_pos);
ScheduledTask *scheduled_task_create(unsigned long second, unsigned long minute, unsigned int hour, unsigned int day, unsigned char weekday, unsigned short month, void (*func)(void *), void *args);
ScheduledTask *scheduler_get_next(ScheduledTaskQueue *queue);
void scheduler_insert(ScheduledTaskQueue *queue, ScheduledTask *task);
ScheduledTaskQueue *scheduler_create();
void scheduler_free(ScheduledTaskQueue *queue);
void scheduled_task_free(ScheduledTask *task);


#endif
