#include "../util.h"

#include "attribute/tests.c"
#include "element-name/tests.c"

int testQueryExecutor() {
  START_TESTS

  status |= testElementNameQuery();
  status |= testAttributeQuery();

  END_TESTS
}
