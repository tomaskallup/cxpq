#include "types.h"

#ifndef NODE_COLLECTION_H

#define NODE_COLLECTION_H

#define NODE_COLLECTION_ALLOC_SIZE 5

NodeCollection *initNodeCollection();
void freeNodeCollection(NodeCollection *collection);
void addNodeToCollection(NodeCollection *collection, XMLNode *node);
/** Returns the number of time the node was present (and removed) */
int removeNodeFromCollection(NodeCollection *collection, XMLNode *node, bool removeAll);
int deduplicateNodeCollection(NodeCollection *collection);
void compactNodeCollection(NodeCollection *collection);
void concatNodeCollection(NodeCollection *collection, NodeCollection *other);

#endif
