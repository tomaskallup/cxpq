#include "util.h"

#include "../query-executor.h"

#define CHECK_RESULT(result, node, index)                                      \
  cmp_mem(result->nodes[index], &node, sizeof(XMLElementNode),                 \
          "Node at index %d should be " #node, index);

static String bookString = (String){"book", 4, 4};
static String book2String = (String){"book2", 5, 5};
static String wildcardString = (String){"*", 1, 1};

static String langString = (String){"lang", 4, 4};
static String enString = (String){"en", 2, 2};
static String czString = (String){"cz", 2, 2};

static struct Attribute langCzAttribute =
    (Attribute){&langString, &czString, false};
static struct Attribute langEnAttribute =
    (Attribute){&langString, &enString, false};

static struct XMLElementNode bookNode = (XMLElementNode){
    ELEMENT,     NULL,

    &bookString, &bookString, NULL, &(Attribute *){&langCzAttribute}, 1, false,
};

static struct XMLElementNode book2Node = (XMLElementNode){
    ELEMENT,      NULL,

    &book2String, &book2String, NULL, &(Attribute *){&langEnAttribute}, 1,
    false,
};

static struct NodeCollection childrenWithBook2Node =
    (NodeCollection){&(XMLNode *){(XMLNode *)&book2Node}, 1, 1};

static struct XMLElementNode book2NodeWithNestedBook2Node = (XMLElementNode){
    ELEMENT,      NULL,

    &book2String, &book2String, &childrenWithBook2Node, NULL, 0, false,
};

static struct XMLElementNode bookNodeWithChild = (XMLElementNode){
    ELEMENT,
    NULL,

    &bookString,
    &bookString,
    &childrenWithBook2Node,
    &(Attribute *){&langCzAttribute},
    1,
    false,
};

static struct XMLDocument bookDocument = (XMLDocument){
    &(NodeCollection){&(XMLNode *){(XMLNode *)&bookNode}, 1, 1},
    0,
};

static struct XMLDocument bookDocumentWithChild = (XMLDocument){
    &(NodeCollection){&(XMLNode *){(XMLNode *)&bookNodeWithChild}, 1, 1},
    0,
};

static struct XMLDocument book2Document = (XMLDocument){
    &(NodeCollection){&(XMLNode *){(XMLNode *)&book2Node}, 1, 1},
    0,
};

static struct XMLDocument book2DocumentNested = (XMLDocument){
    &(NodeCollection){&(XMLNode *){(XMLNode *)&book2NodeWithNestedBook2Node}, 1,
                      1},
    0,
};

void simpleDirectElementQuery() {
  struct Query query = (Query){
      ElementName, &bookString, NULL, NULL, NESTING_DIRECT, NULL, NULL,
  };

  diag(" > Document with matching node\n");
  NodeCollection *result = executeQuery(&bookDocument, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_RESULT(result, bookNode, 0);

  freeNodeCollection(result);

  diag(" > Document without matching node\n");

  result = executeQuery(&book2Document, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 0, "Result size should be 0");
  ok(result->nodes == NULL, "Result nodes should not be allocated");

  freeNodeCollection(result);
}

void simpleNonDirectElementQuery() {
  struct Query query = (Query){
      ElementName, &bookString, NULL, NULL, NESTING_INDIRECT, NULL, NULL,
  };

  struct Query query2 = (Query){
      ElementName, &book2String, NULL, NULL, NESTING_INDIRECT, NULL, NULL,
  };

  diag(" > Document with matching root node\n");
  NodeCollection *result = executeQuery(&bookDocument, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_RESULT(result, bookNode, 0);

  freeNodeCollection(result);

  diag(" > Document with matching nested node\n");

  result = executeQuery(&bookDocumentWithChild, &query2);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_RESULT(result, book2Node, 0);

  freeNodeCollection(result);
}

void wildcardElementQuery() {
  struct Query query = (Query){
      ElementName, &wildcardString, NULL, NULL, NESTING_DIRECT, NULL, NULL,
  };

  diag(" > With direct nesting\n");
  NodeCollection *result = executeQuery(&bookDocumentWithChild, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_RESULT(result, bookNodeWithChild, 0);

  freeNodeCollection(result);

  diag(" > With indirect nesting\n");

  query.nesting = NESTING_INDIRECT;

  result = executeQuery(&bookDocumentWithChild, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 2, "Result size should be 2");
  CHECK_RESULT(result, bookNodeWithChild, 0);
  CHECK_RESULT(result, book2Node, 1);

  freeNodeCollection(result);
}

void simpleNonDirectElementQueryWithNestedDuplicate() {
  struct Query query = (Query){
      ElementName, &book2String, NULL, NULL, NESTING_INDIRECT, NULL, NULL,
  };

  NodeCollection *result = executeQuery(&book2DocumentNested, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 2, "Result size should be 2");
  CHECK_RESULT(result, book2NodeWithNestedBook2Node, 0);
  CHECK_RESULT(result, book2Node, 1);

  freeNodeCollection(result);
}

void nestedDirectElementQuery() {
  struct Query query = (Query){
      ElementName, &bookString, NULL, NULL, NESTING_DIRECT, NULL, NULL,
  };
  struct Query query2 = (Query){
      ElementName, &book2String, NULL, NULL, NESTING_DIRECT, NULL, &query,
  };

  query.next = &query2;

  NodeCollection *result = executeQuery(&bookDocumentWithChild, &query);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_RESULT(result, book2Node, 0);

  freeNodeCollection(result);
}

void simpleAttributeQuery() {
  struct Query queryNoValue = (Query){ElementAttribute, NULL, &langString, NULL,
                                      NESTING_INDIRECT, NULL};

  diag(" > Query with no attribute value");

  NodeCollection *result = executeQuery(&bookDocumentWithChild, &queryNoValue);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 2, "Result size should be 2");
  CHECK_RESULT(result, bookNodeWithChild, 0);
  CHECK_RESULT(result, book2Node, 1);

  freeNodeCollection(result);

  struct Query queryEnValue = (Query){
      ElementAttribute, &enString, &langString, NULL, NESTING_INDIRECT, NULL};

  diag(" > Query with attribute value");

  result = executeQuery(&bookDocumentWithChild, &queryEnValue);

  ok(result != NULL, "Result is not null");
  cmp_ok(result->size, "==", 1, "Result size should be 1");
  CHECK_RESULT(result, book2Node, 0);

  freeNodeCollection(result);
}

int testQueryExecutor() {
  plan(NO_PLAN);

  IT(simpleDirectElementQuery);
  IT(simpleNonDirectElementQuery);
  IT(wildcardElementQuery);
  IT(simpleNonDirectElementQueryWithNestedDuplicate);

  IT(nestedDirectElementQuery);

  IT(simpleAttributeQuery);

  done_testing();
}
