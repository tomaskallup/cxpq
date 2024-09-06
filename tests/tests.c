#include "../lib/libtap/tap.h"

#include "util.h"

#include "./node-collection/tests.c"
#include "./query-executor/tests.c"

int main(int argc, char *argv[]) {
  START_TESTS

  RUN_TESTS(testNodeCollection);

  RUN_TESTS(testQueryExecutor);

  // Use fmemopen (or tmpfile) for FILE mocking when testing parser

  END_TESTS
}
