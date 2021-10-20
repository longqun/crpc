#ifndef CRPC_UTILLS_H
#define CRPC_UTILLS_H
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <fcntl.h>
#include <stdint.h>
namespace crpc
{

void print_stacktrace();

uint64_t get_timestamps_ms();

}

#endif