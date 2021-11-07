#ifndef CRPC_UTILLS_H
#define CRPC_UTILLS_H
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string>
namespace crpc
{

void read_all_fd(int fd);

int createEventfd();

void print_stacktrace();

uint64_t get_timestamps_ms();

std::string get_http_code_name(int http_code);

}

#endif