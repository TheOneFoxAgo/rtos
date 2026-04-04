#include <stdarg.h>
#include <stdio.h>
#include "sys.h"

int Log(const char* format, ...) {
  int res = 0;
  va_list args;
  va_start(args, format);
  res = vfprintf(stderr, format, args);
  va_end(args);
  return res;
}
