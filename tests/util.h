#include "../lib/libtap/tap.h"

#ifndef TESTS_UTIL_H

#define CHECK_NODE_IN_COLLECTION(result, node, index)                          \
  cmp_mem(result->nodes[index], &node, sizeof(node),                           \
          "Node at index %d should be " #node, index);

#define PRINT_SEPARATOR() diag("----------------------------------------\n");

#define START_TESTS int status = 0;
#define END_TESTS return status;

#define RUN_TESTS(func)                                                        \
  {                                                                            \
                                                                               \
    PRINT_SEPARATOR();                                                         \
    diag("Testing \"" #func "\"\n");                                           \
    PRINT_SEPARATOR();                                                         \
    int result = func();                                                       \
    status |= result;                                                          \
    if (result != 0) {                                                         \
      PRINT_SEPARATOR();                                                       \
      diag("Failed \"" #func "\"\n");                                          \
      PRINT_SEPARATOR();                                                       \
      return status;                                                           \
    }                                                                          \
    PRINT_SEPARATOR();                                                         \
    diag("Finished \"" #func "\"\n");                                          \
    PRINT_SEPARATOR();                                                         \
  };

#define IT(func)                                                               \
  diag("Runnning test " #func "\n");                                           \
  func();

#endif
