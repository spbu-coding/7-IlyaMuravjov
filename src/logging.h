#ifndef TASK_7_LOGGING_STD_WRAPPERS_H
#define TASK_7_LOGGING_STD_WRAPPERS_H

#include <stdio.h>

#define LOG_ERROR(...) (fprintf(stderr, __VA_ARGS__))

#endif //TASK_7_LOGGING_STD_WRAPPERS_H
