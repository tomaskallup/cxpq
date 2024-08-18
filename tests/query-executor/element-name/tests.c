#include "../../../query-executor.h"

#include "../../util.h"

#include "fixtures.c"

void simpleDirectElementQuery() {
  struct XMLDocument bookDocument = (XMLDocument){
      &(NodeCollection){&(XMLNode *){(XMLNode *)&elementNameQueryFixtures.bookNode}, 1,
                        1},
      0,
  };

  struct XMLDocument book2Document = (XMLDocument){
      &(NodeCollection){&(XMLNode *){(XMLNode *)&elementNameQueryFixtures.book2Node},
                        1, 1},
      0,
  };

  struct Query query = (Query){
      ElementName,    &elementNameQueryFixtures.bookString,
      NULL,           NULL,
      NESTING_DIRECT, NULL,
      NULL,
  };

  diag(" > Document with matching node\n");
  NodeCollection *result = executeQuery(&bookDocument, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_NODE_IN_COLLECTION(result, elementNameQueryFixtures.bookNode, 0);

  freeNodeCollection(result);

  diag(" > Document without matching node\n");

  result = executeQuery(&book2Document, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 0, "Result size should be 0");
  ok(result->nodes == NULL, "Result nodes should not be allocated");

  freeNodeCollection(result);
}

void simpleNonDirectElementQuery() {
  struct XMLDocument bookDocument = (XMLDocument){
      &(NodeCollection){&(XMLNode *){(XMLNode *)&elementNameQueryFixtures.bookNode}, 1,
                        1},
      0,
  };
  struct XMLDocument bookDocumentWithChild = (XMLDocument){
      &(NodeCollection){
          &(XMLNode *){(XMLNode *)&elementNameQueryFixtures.bookNodeWithChild}, 1, 1},
      0,
  };

  struct Query query = (Query){
      ElementName,
      &elementNameQueryFixtures.bookString,
      NULL,
      NULL,
      NESTING_INDIRECT,
      NULL,
      NULL,
  };

  struct Query query2 = (Query){
      ElementName,
      &elementNameQueryFixtures.book2String,
      NULL,
      NULL,
      NESTING_INDIRECT,
      NULL,
      NULL,
  };

  diag(" > Document with matching root node\n");
  NodeCollection *result = executeQuery(&bookDocument, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_NODE_IN_COLLECTION(result, elementNameQueryFixtures.bookNode, 0);

  freeNodeCollection(result);

  diag(" > Document with matching nested node\n");

  result = executeQuery(&bookDocumentWithChild, &query2);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_NODE_IN_COLLECTION(result, elementNameQueryFixtures.book2Node, 0);

  freeNodeCollection(result);
}

void wildcardElementQuery() {
  struct XMLDocument bookDocumentWithChild = (XMLDocument){
      &(NodeCollection){
          &(XMLNode *){(XMLNode *)&elementNameQueryFixtures.bookNodeWithChild}, 1, 1},
      0,
  };

  struct Query query = (Query){
      ElementName,    &elementNameQueryFixtures.wildcardString,
      NULL,           NULL,
      NESTING_DIRECT, NULL,
      NULL,
  };

  diag(" > With direct nesting\n");
  NodeCollection *result = executeQuery(&bookDocumentWithChild, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_NODE_IN_COLLECTION(result, elementNameQueryFixtures.bookNodeWithChild, 0);

  freeNodeCollection(result);

  diag(" > With indirect nesting\n");

  query.nesting = NESTING_INDIRECT;

  result = executeQuery(&bookDocumentWithChild, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 2, "Result size should be 2");
  CHECK_NODE_IN_COLLECTION(result, elementNameQueryFixtures.bookNodeWithChild, 0);
  CHECK_NODE_IN_COLLECTION(result, elementNameQueryFixtures.book2Node, 1);

  freeNodeCollection(result);
}

void simpleNonDirectElementQueryWithNestedDuplicate() {
  struct XMLDocument book2DocumentNested = (XMLDocument){
      &(NodeCollection){
          &(XMLNode *){
              (XMLNode *)&elementNameQueryFixtures.book2NodeWithNestedBook2Node},
          1, 1},
      0,
  };

  struct Query query = (Query){
      ElementName,
      &elementNameQueryFixtures.book2String,
      NULL,
      NULL,
      NESTING_INDIRECT,
      NULL,
      NULL,
  };

  NodeCollection *result = executeQuery(&book2DocumentNested, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 2, "Result size should be 2");
  CHECK_NODE_IN_COLLECTION(result,
                           elementNameQueryFixtures.book2NodeWithNestedBook2Node, 0);
  CHECK_NODE_IN_COLLECTION(result, elementNameQueryFixtures.book2Node, 1);

  freeNodeCollection(result);
}

void nestedDirectElementQuery() {
  struct XMLDocument bookDocumentWithChild = (XMLDocument){
      &(NodeCollection){
          &(XMLNode *){(XMLNode *)&elementNameQueryFixtures.bookNodeWithChild}, 1, 1},
      0,
  };

  struct Query query = (Query){
      ElementName,    &elementNameQueryFixtures.bookString,
      NULL,           NULL,
      NESTING_DIRECT, NULL,
      NULL,
  };
  struct Query query2 = (Query){
      ElementName,    &elementNameQueryFixtures.book2String,
      NULL,           NULL,
      NESTING_DIRECT, NULL,
      &query,
  };

  query.next = &query2;

  NodeCollection *result = executeQuery(&bookDocumentWithChild, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_NODE_IN_COLLECTION(result, elementNameQueryFixtures.book2Node, 0);

  freeNodeCollection(result);
}

int testElementNameQuery() {
  plan(NO_PLAN);

  IT(simpleDirectElementQuery);
  IT(simpleNonDirectElementQuery);
  IT(wildcardElementQuery);
  IT(simpleNonDirectElementQueryWithNestedDuplicate);

  IT(nestedDirectElementQuery);

  done_testing();
}
