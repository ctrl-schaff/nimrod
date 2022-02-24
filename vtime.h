#ifndef VTIME_H_
#define VTIME_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int msleep(long msec);
int nanosleep(const struct timespec *req, struct timespec *rem);
#endif // VTIME_H
