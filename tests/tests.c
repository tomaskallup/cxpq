#include "../lib/libtap/tap.h"

#include "test-query-executor.c"
#include "test-node-collection.c"

#define PRINT_SEPARATOR() diag("----------------------------------------\n");

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

int main(int argc, char *argv[]) {
  int status = 0;

  RUN_TESTS(testNodeCollection);

  RUN_TESTS(testQueryExecutor);

  // Use fmemopen for FILE mocking when testing parser

  return status;
}
