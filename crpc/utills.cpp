#include "utills.h"

namespace crpc
{

void print_stacktrace()
{
    int size = 50;
    void * array[50];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    for (int i = 0; i < stack_num; ++i)
    {
        printf("%s\n", stacktrace[i]);
    }
    free(stacktrace);
}

uint64_t get_timestamps_ms()
{
    struct timeval current;
    gettimeofday(&current, NULL);
    uint64_t ret = current.tv_sec * 1000 + current.tv_usec / 1000;
    return ret;
}

}
