#ifndef LOGGING_H
#define LOGGING_H

#ifdef DEBUG

#include <stdio.h>

#define Log(...)                                     \
  fprintf(stderr, "  %s(%d): ", __func__, __LINE__); \
  fprintf(stderr, __VA_ARGS__);
#else
#define Log(...) (void)(0)
#endif

#endif
