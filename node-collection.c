#include <stdlib.h>
#include <string.h>

#include "node-collection.h"
#include "util.h"

NodeCollection *initNodeCollection(void) {
  NodeCollection *collection = malloc(sizeof(NodeCollection));
  collection->nodes = NULL;
  collection->size = 0;
  collection->allocated = 0;
  collection->lastIndex = 0;

  return collection;
}

void freeNodeCollection(NodeCollection *collection) {
  if (collection->nodes)
    free(collection->nodes);

  free(collection);
}

void addNodeToCollection(NodeCollection *collection, XMLNode *node) {
  bool isFirstNode = collection->size == 0 && collection->lastIndex == 0;
  if (!isFirstNode)
    collection->lastIndex++;
  collection->size++;

  if (collection->allocated == 0 ||
      collection->lastIndex >= collection->allocated) {
    collection->allocated += NODE_COLLECTION_ALLOC_SIZE;
    collection->nodes =
        realloc(collection->nodes, collection->allocated * sizeof(XMLNode *));

    // Make sure we `NULL` all newly allocated nodes
    for (size_t i = collection->lastIndex + 1; i < collection->allocated; i++) {
      collection->nodes[i] = NULL;
    }
  }

  collection->nodes[collection->lastIndex] = node;
}

void compactNodeCollection(NodeCollection *collection) {
  if (collection->size == 0) {
    collection->allocated = 0;
    collection->lastIndex = 0;

    if (collection->nodes) {
      free(collection->nodes);
      collection->nodes = NULL;
    }

    return;
  }
  for (size_t i = 0; i <= collection->lastIndex; i++) {
    if (!collection->nodes[i]) {
      size_t oldIndex = i;
      size_t newIndex = i;

      XMLNode *nextNonNull = NULL;
      while (!nextNonNull && ++newIndex <= collection->lastIndex) {
        nextNonNull = collection->nodes[newIndex];
      }

      if (nextNonNull) {
        collection->nodes[oldIndex] = nextNonNull;
        collection->nodes[newIndex] = NULL;
      }
    }
  }

  collection->nodes =
      realloc(collection->nodes, collection->size * sizeof(XMLNode *));
  collection->allocated = collection->size;
  collection->lastIndex = collection->allocated - 1;
}

int removeNodeFromCollection(NodeCollection *collection, XMLNode *node,
                             bool removeAll) {
  size_t found = 0;

  for (size_t i = 0; i < collection->allocated; i++) {
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

  size_t spaceLeft = collection->allocated > 0
                         ? collection->allocated - (collection->lastIndex + 1)
                         : 0;
  size_t spaceNeeded = other->allocated - spaceLeft;
  if (spaceNeeded > 0) {
    collection->allocated += spaceNeeded;

    collection->nodes =
        realloc(collection->nodes, collection->allocated * sizeof(XMLNode *));
  }

  size_t start = collection->size == 0 && collection->lastIndex == 0
                     ? 0
                     : collection->lastIndex + 1;

  // We intentionally copy all allocated nodes (including NULLs)
  for (size_t i = start; i < collection->allocated; i++) {
    collection->nodes[i] = other->nodes[i - start];
  }

  // Make sure to account for two `0` last indexes
  if (collection->size > 0) collection->lastIndex++;

  collection->lastIndex += other->lastIndex;
  collection->size += other->size;
}

NodeCollection *cloneNodeCollection(NodeCollection *collection) {
  NodeCollection *copy = malloc(sizeof(NodeCollection));

  copy->size = collection->size;
  copy->allocated = collection->allocated;
  copy->lastIndex = collection->lastIndex;

  if (copy->allocated == 0) return copy;

  copy->nodes = calloc(copy->allocated, sizeof(XMLNode *));

  // We intentionally copy all allocated nodes (including NULLs)
  for (size_t i = 0; i < copy->allocated; i++) {
    copy->nodes[i] = collection->nodes[i];
  }

  return copy;
}

void printNodeCollection(NodeCollection *collection, bool includeNewLine) {
  if (collection->size == 0) {
    printf("\n");
    return;
  }

  for (size_t i = 0; i <= collection->lastIndex; i++) {
    XMLNode *node = collection->nodes[i];
    if (!node)
      continue;
    switch (node->type) {
    case ELEMENT: {
      XMLElementNode *elementNode = (XMLElementNode *)node;
      printf("<%s", elementNode->tag->value);

      for (size_t a = 0; a < elementNode->attributesSize; a++) {
        struct Attribute *attribute = elementNode->attributes[a];
        printf(" %s=\"%s\"", attribute->name->value, attribute->content->value);
      }

      printf(">");

      if (elementNode->children->size > 0)
        printNodeCollection(elementNode->children, false);

      printf("</%s>", elementNode->tag->value);
      break;
    }
    case COMMENT: {
      XMLCommentNode *commentNode = (XMLCommentNode *)node;
      printf("<!--%s-->", commentNode->content->value);

      break;
    }
    case TEXT: {
      XMLTextNode *textNode = (XMLTextNode *)node;
      if (textNode->isCDATA)
        printf("<![CDATA[");
      printf("%s", textNode->content->value);
      if (textNode->isCDATA)
        printf("]]>");

      break;
    }
    case DTD: {
      XMLDTDNode *dtdNode = (XMLDTDNode *)node;
      printf("<!DOCTYPE %s", dtdNode->name->value);

      if (dtdNode->systemID)
        printf(" SYSTEM \"%s\"", dtdNode->systemID->value);
      else if (dtdNode->content)
        printf(" [%s]", dtdNode->content->value);

      printf(">");

      break;
    }
    case PROCESSING_INSTRUCTION: {
      XMLProcessingInstructionNode *processingInstructionNode =
          (XMLProcessingInstructionNode *)node;
      printf("<?%s", processingInstructionNode->tag->value);

      for (size_t a = 0; a < processingInstructionNode->attributesSize; a++) {
        struct Attribute *attribute = processingInstructionNode->attributes[a];
        printf(" %s=\"%s\"", attribute->name->value, attribute->content->value);
      }

      printf("?>");
      break;
    }
    }
    if (includeNewLine)
      printf("\n");
  }
}
