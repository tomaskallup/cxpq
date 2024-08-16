#include "util.h"

#include "../node-collection.h"

static String testString = (String){"test", 4, 4};
static String test2String = (String){"test2", 5, 5};

static struct XMLElementNode testNode = (XMLElementNode){
    ELEMENT,     NULL,

    &testString, &testString, NULL, NULL, 0, false,
};

static struct XMLElementNode testNode2 = (XMLElementNode){
    ELEMENT,     NULL,

    &testString, &testString, NULL, NULL, 0, false,
};

static struct XMLElementNode test2Node = (XMLElementNode){
    ELEMENT,      NULL,

    &test2String, &test2String, NULL, NULL, 0, false,
};

void testInitNodeCollection() {
  NodeCollection *collection = initNodeCollection();

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 0, "Collection size should be 0");
  cmp_ok(collection->allocated, "==", 0,
         "Collection should have 0 nodes allocated");
  ok(collection->nodes == NULL, "Collection nodes should be null");

  freeNodeCollection(collection);
}

void testAddNodeToCollection() {
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&testNode);
  addNodeToCollection(collection, (XMLNode *)&testNode2);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 2, "Collection size should be 2");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  cmp_mem(collection->nodes[0], &testNode, sizeof(XMLElementNode),
          "First node should be testNode");
  cmp_mem(collection->nodes[1], &testNode2, sizeof(XMLElementNode),
          "Second node should be testNode2");

  freeNodeCollection(collection);
}

void testRemoveNodefromCollection() {
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&testNode);
  addNodeToCollection(collection, (XMLNode *)&testNode2);
  addNodeToCollection(collection, (XMLNode *)&test2Node);

  removeNodeFromCollection(collection, (XMLNode *)&testNode, true);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 2, "Collection size should be 2");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  ok(collection->nodes[0] == NULL, "First node should be null");
  cmp_mem(collection->nodes[1], &testNode2, sizeof(XMLElementNode),
          "Second node should be testNode2");
  cmp_mem(collection->nodes[2], &test2Node, sizeof(XMLElementNode),
          "Third node should be test2Node");

  freeNodeCollection(collection);
}

void testRemoveNodefromCollectionWithDuplicate() {
  diag(" > With removeAll se to true");
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&testNode2);
  addNodeToCollection(collection, (XMLNode *)&testNode);
  addNodeToCollection(collection, (XMLNode *)&test2Node);
  addNodeToCollection(collection, (XMLNode *)&testNode);

  int result = removeNodeFromCollection(collection, (XMLNode *)&testNode, true);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 2, "Collection size should be 2");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  cmp_mem(collection->nodes[0], &testNode2, sizeof(XMLElementNode),
          "First node should be testNode2");
  ok(collection->nodes[1] == NULL, "Second node should be null");
  cmp_mem(collection->nodes[2], &test2Node, sizeof(XMLElementNode),
          "Third node should be test2Node");
  ok(collection->nodes[3] == NULL, "Fourth node should be null");
  cmp_ok(result, "==", 2, "Result should be 2 nodes removed");

  freeNodeCollection(collection);

  diag(" > With removeAll se to false");
  collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&testNode2);
  addNodeToCollection(collection, (XMLNode *)&testNode);
  addNodeToCollection(collection, (XMLNode *)&test2Node);
  addNodeToCollection(collection, (XMLNode *)&testNode);

  result = removeNodeFromCollection(collection, (XMLNode *)&testNode, false);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 3, "Collection size should be 3");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  cmp_mem(collection->nodes[0], &testNode2, sizeof(XMLElementNode),
          "First node should be testNode2");
  ok(collection->nodes[1] == NULL, "Second node should be null");
  cmp_mem(collection->nodes[2], &test2Node, sizeof(XMLElementNode),
          "Third node should be test2Node");
  cmp_mem(collection->nodes[3], &testNode, sizeof(XMLElementNode),
          "Fourth node should be testNode");
  cmp_ok(result, "==", 1, "Result should be 1 nodes removed");

  freeNodeCollection(collection);
}

void testCompactNodeCollection() {
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&testNode2);
  addNodeToCollection(collection, (XMLNode *)NULL);
  addNodeToCollection(collection, (XMLNode *)&test2Node);
  addNodeToCollection(collection, (XMLNode *)NULL);
  addNodeToCollection(collection, (XMLNode *)NULL);
  addNodeToCollection(collection, (XMLNode *)&testNode);
  addNodeToCollection(collection, (XMLNode *)&testNode2);

  collection->size = 4;

  compactNodeCollection(collection);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 4, "Collection size should be 4");
  cmp_ok(collection->allocated, "==", 4,
         "Collection should have 4 nodes allocated");
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  cmp_mem(collection->nodes[0], &testNode2, sizeof(XMLElementNode),
          "First node should be testNode2");
  cmp_mem(collection->nodes[1], &test2Node, sizeof(XMLElementNode),
          "Second node should be test2Node");
  cmp_mem(collection->nodes[2], &testNode, sizeof(XMLElementNode),
          "Third node should be testNode");
  cmp_mem(collection->nodes[3], &testNode2, sizeof(XMLElementNode),
          "Fourth node should be testNode2");

  freeNodeCollection(collection);
}

int testNodeCollection() {
  plan(NO_PLAN);

  IT(testInitNodeCollection);

  IT(testAddNodeToCollection);

  IT(testRemoveNodefromCollection);
  IT(testRemoveNodefromCollectionWithDuplicate);

  IT(testCompactNodeCollection);

  done_testing();
}
