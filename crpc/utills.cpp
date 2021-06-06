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

}
