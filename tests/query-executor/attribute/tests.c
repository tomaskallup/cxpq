#include "../../../query-executor.h"

#include "../../util.h"

#include "fixtures.c"

void simpleAttributeQuery() {
  struct Query queryNoValue =
      (Query){ElementAttribute, NULL, &attributeQueryFixtures.langString, NULL,
              NESTING_INDIRECT, NULL};
  struct XMLDocument bookDocumentWithChild = (XMLDocument){
      &(NodeCollection){
          &(XMLNode *){(XMLNode *)&attributeQueryFixtures.bookNodeWithChild}, 1,
          1},
      0,
  };

  diag(" > Query with no attribute value");

  NodeCollection *result = executeQuery(&bookDocumentWithChild, &queryNoValue);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 2, "Result size should be 2");
  CHECK_NODE_IN_COLLECTION(result, attributeQueryFixtures.bookNodeWithChild, 0);
  CHECK_NODE_IN_COLLECTION(result, attributeQueryFixtures.book2Node, 1);

  freeNodeCollection(result);

  struct Query queryEnValue = (Query){ElementAttribute,
                                      &attributeQueryFixtures.enString,
                                      &attributeQueryFixtures.langString,
                                      NULL,
                                      NESTING_INDIRECT,
                                      NULL};

  diag(" > Query with attribute value");

  result = executeQuery(&bookDocumentWithChild, &queryEnValue);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_NODE_IN_COLLECTION(result, attributeQueryFixtures.book2Node, 0);

  freeNodeCollection(result);
}

int testAttributeQuery() {
  plan(NO_PLAN);

  IT(simpleAttributeQuery);

  done_testing();
}
