#include "../lib/libtap/tap.h"

#ifndef TESTS_UTIL_H

#define IT(func)                                                               \
  diag("Runnning test " #func "\n");                                           \
  func();

#endif
