#include "../util.h"

#include "../../node-collection.h"
#include "../../util.h"

#include "./fixtures.c"

void testInitNodeCollection() {
  NodeCollection *collection = initNodeCollection();

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 0, "Collection size should be 0");
  cmp_ok(collection->allocated, "==", 0,
         "Collection should have 0 nodes allocated");
  ok(!collection->nodes, "Collection nodes should be null");

  freeNodeCollection(collection);
}

void testAddNodeToCollection() {
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode2);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 2, "Collection size should be 2");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode, 0);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode2, 1);

  freeNodeCollection(collection);
}

void testRemoveNodefromCollection() {
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode2);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.test2Node);

  removeNodeFromCollection(collection,
                           (XMLNode *)&nodeCollectionFixtures.testNode, true);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 2, "Collection size should be 2");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  CHECK_NODE_IN_COLLECTION(collection, *NULL, 0);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode2, 1);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.test2Node, 2);

  freeNodeCollection(collection);
}

void testRemoveNodefromCollectionWithDuplicate() {
  diag(" > With removeAll se to true");
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode2);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.test2Node);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode);

  int result = removeNodeFromCollection(
      collection, (XMLNode *)&nodeCollectionFixtures.testNode, true);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 2, "Collection size should be 2");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode2, 0);
  CHECK_NODE_IN_COLLECTION(collection, *NULL, 1);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.test2Node, 2);
  CHECK_NODE_IN_COLLECTION(collection, *NULL, 3);
  cmp_ok(result, "==", 2, "Result should be 2 nodes removed");

  freeNodeCollection(collection);

  diag(" > With removeAll se to false");
  collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode2);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.test2Node);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode);

  result = removeNodeFromCollection(
      collection, (XMLNode *)&nodeCollectionFixtures.testNode, false);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 3, "Collection size should be 3");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");

  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode2, 0);
  CHECK_NODE_IN_COLLECTION(collection, *NULL, 1);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.test2Node, 2);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode, 3);
  cmp_ok(result, "==", 1, "Result should be 1 nodes removed");

  freeNodeCollection(collection);
}

void testCompactNodeCollection() {
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode2);
  addNodeToCollection(collection, (XMLNode *)NULL);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.test2Node);
  addNodeToCollection(collection, (XMLNode *)NULL);
  addNodeToCollection(collection, (XMLNode *)NULL);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode2);

  collection->size = 4;

  compactNodeCollection(collection);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 4, "Collection size should be 4");
  cmp_ok(collection->allocated, "==", 4,
         "Collection should have 4 nodes allocated");
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode2, 0);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.test2Node, 1);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode, 2);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode2, 3);

  freeNodeCollection(collection);
}

void testCloneNodeCollection() {
  NodeCollection *collection = initNodeCollection();

  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.testNode2);
  addNodeToCollection(collection, (XMLNode *)&nodeCollectionFixtures.test2Node);

  NodeCollection *copy = cloneNodeCollection(collection);

  ok(collection != NULL, "Collection is not null");
  cmp_ok(collection->size, "==", 3, "Collection size should be 3");
  cmp_ok(collection->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Collection should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(collection->nodes != NULL, "Collection nodes should not be null");
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode, 0);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.testNode2, 1);
  CHECK_NODE_IN_COLLECTION(collection, nodeCollectionFixtures.test2Node, 2);

  freeNodeCollection(collection);

  ok(copy != NULL, "Copy is not null");
  cmp_ok(copy->size, "==", 3, "Copy size should be 3");
  cmp_ok(copy->allocated, "==", NODE_COLLECTION_ALLOC_SIZE,
         "Copy should have %i nodes allocated",
         NODE_COLLECTION_ALLOC_SIZE);
  ok(copy->nodes != NULL, "Copy nodes should not be null");
  CHECK_NODE_IN_COLLECTION(copy, nodeCollectionFixtures.testNode, 0);
  CHECK_NODE_IN_COLLECTION(copy, nodeCollectionFixtures.testNode2, 1);
  CHECK_NODE_IN_COLLECTION(copy, nodeCollectionFixtures.test2Node, 2);

  freeNodeCollection(copy);
}

int testNodeCollection() {
  plan(NO_PLAN);

  IT(testInitNodeCollection);

  IT(testAddNodeToCollection);

  IT(testRemoveNodefromCollection);
  IT(testRemoveNodefromCollectionWithDuplicate);

  IT(testCompactNodeCollection);

  IT(testCloneNodeCollection);

  done_testing();
}
