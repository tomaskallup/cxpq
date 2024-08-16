#include <stdlib.h>

#include "node-collection.h"
#include "util.h"

NodeCollection *initNodeCollection() {
  NodeCollection *collection = malloc(sizeof(NodeCollection));
  collection->nodes = NULL;
  collection->size = 0;
  collection->allocated = 0;

  return collection;
}

void freeNodeCollection(NodeCollection *collection) {
  if (collection->nodes != NULL)
    free(collection->nodes);

  free(collection);
}

void addNodeToCollection(NodeCollection *collection, XMLNode *node) {
  collection->size++;

  if (collection->size > collection->allocated) {
    collection->allocated += NODE_COLLECTION_ALLOC_SIZE;
    collection->nodes =
        realloc(collection->nodes, collection->allocated * sizeof(XMLNode *));
  }

  collection->nodes[collection->size - 1] = node;
}

void compactNodeCollection(NodeCollection *collection) {
  for (int i = 0; i < collection->allocated; i++) {
    if (collection->nodes[i] == NULL) {
      int oldIndex = i;
      int newIndex = i;

      XMLNode *nextNonNull = NULL;
      while (nextNonNull == NULL && ++newIndex < collection->allocated) {
        nextNonNull = collection->nodes[newIndex];
      }

      if (nextNonNull != NULL) {
        collection->nodes[oldIndex] = nextNonNull;
        collection->nodes[newIndex] = NULL;
      }
    }
  }

  collection->nodes =
      realloc(collection->nodes, collection->size * sizeof(XMLNode *));
  collection->allocated = collection->size;
}

int removeNodeFromCollection(NodeCollection *collection, XMLNode *node,
                             bool removeAll) {
  int found = 0;

  for (int i = 0; i < collection->size; i++) {
    if (collection->nodes[i] == node) {
      found++;
      collection->nodes[i] = NULL;
      if (!removeAll)
        break;
    }
  }

  collection->size -= found;

  return found;
}

void concatNodeCollection(NodeCollection *collection, NodeCollection *other) {
  if (other->size == 0)
    return;

  collection->size += other->size;
  if (collection->size > collection->allocated) {
    collection->allocated = collection->size;

    collection->nodes =
        realloc(collection->nodes, collection->allocated * sizeof(XMLNode *));
  }

  for (int i = 0; i < other->size; i++) {
    collection->nodes[collection->size - 1 - i] = other->nodes[i];
  }
}
