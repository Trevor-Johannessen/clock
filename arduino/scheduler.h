#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <NTPClient.h>
#include "linked-list.h"

typedef struct {
  Link *task_list;
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

#define CLOCK_BASE 0x0
#define SCHEDTASK_ONEOFF 0x1

typedef struct {
  unsigned char oneoff: 1;
} ScheduledTaskSettings;

/*
  For each value in the cron flip the corresponding bits in the number
  Example:
    December -> month |= 1<<(12-1)
*/
typedef struct {
  unsigned long long second;
  unsigned long long minute;
  unsigned int hour;
  unsigned int day;
  unsigned char weekday;
    union {
    ScheduledTaskSettings bits;
    char vector;
  } settings;
  unsigned short month;
  unsigned long long next_trigger;
  void (*func)(void *);
  void *args;
} ScheduledTask;

extern NTPClient timeClient;
const int seconds_in_day = 86400;
const long long seconds_in_year = 31536000;
const char days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

Date *scheduler_current_date();
unsigned int scheduler_delta(ScheduledTask *task);
unsigned char scheduler_nearest_bit(long long vector);
void scheduler_check_task(ScheduledTaskQueue *queue);
unsigned char scheduler_delta_vector(char vector_size, long long vector, char start_pos);
ScheduledTask *scheduled_task_create(unsigned long long second, unsigned long long minute, unsigned int hour, unsigned int day, unsigned char weekday, unsigned short month, void (*func)(void *), void *args, unsigned char settings);
ScheduledTask *scheduler_get_next(ScheduledTaskQueue *queue);
void scheduler_insert(ScheduledTaskQueue *queue, ScheduledTask *task);
ScheduledTaskQueue *scheduler_create();
void scheduler_free(ScheduledTaskQueue *queue);
void scheduled_task_free(ScheduledTask *task);
int scheduler_date_to_seconds(Date date);
void date_free(Date *date);


#endif
