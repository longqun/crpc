#ifndef CRPC_LOG_H
#define CRPC_LOG_H

#include <stdio.h>

#define crpc_log(format, ...) \
          printf("[%s:%d->%s] " format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif
