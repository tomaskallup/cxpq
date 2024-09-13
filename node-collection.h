#include "types.h"

#ifndef NODE_COLLECTION_H

#define NODE_COLLECTION_H

#define NODE_COLLECTION_ALLOC_SIZE 5

NodeCollection *initNodeCollection(void);
NodeCollection *cloneNodeCollection(NodeCollection *collection);
void freeNodeCollection(NodeCollection *collection);
void addNodeToCollection(NodeCollection *collection, XMLNode *node);
/** Returns the number of time the node was present (and removed) */
unsigned int removeNodeFromCollection(NodeCollection *collection, XMLNode *node, bool removeAll);
void compactNodeCollection(NodeCollection *collection);
void concatNodeCollection(NodeCollection *collection, NodeCollection *other);

void printNodeCollection(NodeCollection *collection, bool includeNewLine);

#endif
