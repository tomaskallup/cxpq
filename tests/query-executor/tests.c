#include "../util.h"

#include "element-name/tests.c"
#include "attribute/tests.c"

int testQueryExecutor() {
  START_TESTS

  status |= testElementNameQuery();
  status |= testAttributeQuery();

  END_TESTS
}
