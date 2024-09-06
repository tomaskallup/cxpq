#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "query-executor.h"
#include "util.h"

bool compare(String *leftSide, String *comparator, String *rightSide) {
  if (stringEqualChars(comparator, "="))
    return stringEqual(leftSide, rightSide);
  if (stringEqualChars(comparator, "!="))
    return !stringEqual(leftSide, rightSide);

  // We probably need to work with numbers
  double left = atof(leftSide->value);
  double right = atof(rightSide->value);

  if (stringEqualChars(comparator, ">"))
    return left > right;
  if (stringEqualChars(comparator, "<"))
    return left < right;
  if (stringEqualChars(comparator, ">="))
    return left >= right;
  if (stringEqualChars(comparator, "<="))
    return left <= right;

  return false;
  PRINT_ERROR("Unknown comparator %s\n", comparator->value);
}

bool checkNode(XMLElementNode *node, Query *query) {
  if (!query->prev && query->type != ElementName &&
      query->type != ElementAttribute) {
    PRINT_ERROR(
        "First query should be ElementName or ElementAttribute filter\n");
    return false;
  }
  switch (query->type) {
  case ElementName:
    if (query->value->length == 1 && query->value->value[0] == '*')
      return true;

    return stringEqual(node->tag, query->value);
  case ElementAttribute: {
    if (node->attributesSize == 0)
      return false;
    const bool isWildcard =
        query->key->length == 1 && query->key->value[0] == '*';
    const bool hasValue = query->value && query->value->length > 0;

    if (isWildcard && !hasValue)
      return true;

    for (size_t i = 0; i < node->attributesSize; i++) {
      Attribute *attribute = node->attributes[i];
      if (isWildcard || stringEqual(attribute->name, query->key)) {
        if (!hasValue || stringEqual(attribute->content, query->value))
          return true;
        else
          return false;
      }
    }

    return false;
  }
  case FunctionFilter:
    assert(false && "FunctionFilter is not implemented");
    return false;
  }

  return false;
}

// <book lang="cs"><book lang="en">test</book></book>
// `/book` => <book lang="cs">
// `//book` => <book lang="cs">,<book lang="en">
// `//book[@lang=en]` => <book lang="en">
// `//book[@lang=cs]` => <book lang="cs">

NodeCollection *runQuery(NodeCollection *nodes, Query *query,
                         bool checkNodeDirectly) {
  NodeCollection *result = initNodeCollection();

  if (nodes->size == 0)
    return result;

  bool isDirect = query->nesting == NESTING_DIRECT;
  bool hasNesting = query->nesting & NESTING_SOME;

  for (size_t i = 0; i <= nodes->lastIndex; i++) {
    const XMLNode *rawNode = nodes->nodes[i];
    if (!rawNode || rawNode->type != ELEMENT)
      continue;
    XMLElementNode *node = (XMLElementNode *)rawNode;

    if (checkNodeDirectly && checkNode(node, query))
      addNodeToCollection(result, (XMLNode *)node);

    // If we need to "descend", we check children
    if (!isDirect && hasNesting && node->children) {
      NodeCollection *childrenResult = runQuery(node->children, query, true);
      if (childrenResult->size > 0)
        concatNodeCollection(result, childrenResult);
      freeNodeCollection(childrenResult);
    }
  }

  if (result->size == 0)
    return result;

  if (query->subQueryCount > 0) {
    for (size_t i = 0; i < query->subQueryCount; i++) {
      Query *subQuery = query->subQueries[i];
      NodeCollection *resultCopy = cloneNodeCollection(result);
      for (size_t i = 0; i < resultCopy->allocated; i++) {
        XMLNode *currentNode = resultCopy->nodes[i];

        if (!currentNode)
          continue;

        NodeCollection elementOnly = {
            .size = 1,
            .allocated = 1,
            .nodes = &currentNode,
        };
        NodeCollection *newResult = runQuery(
            &elementOnly, subQuery,
            // Subquery should be executed on children if it's ElementName,
            // otherwise it should directly check the node itself
            subQuery->type != ElementName);

        if (newResult->size == 0)
          removeNodeFromCollection(resultCopy, currentNode, true);
        freeNodeCollection(newResult);
      }

      freeNodeCollection(result);
      result = resultCopy;
    }
  }

  Query *nextQuery = query->next;
  if (nextQuery) {
    if (nextQuery->nesting & NESTING_SOME) {
      NodeCollection *childrenNodes = initNodeCollection();
      for (size_t i = 0; i < result->allocated; i++) {
        XMLNode *node = result->nodes[i];
        if (!node || node->type != ELEMENT)
          continue;
        XMLElementNode *elementNode = (XMLElementNode *)node;
        if (elementNode->children)
          concatNodeCollection(childrenNodes, elementNode->children);
      }

      freeNodeCollection(result);
      result = runQuery(childrenNodes, nextQuery, true);
      freeNodeCollection(childrenNodes);
    }
  }

  return result;
}

NodeCollection *executeQuery(XMLDocument *document, Query *query) {
  NodeCollection *nodes = initNodeCollection();

  XMLElementNode *root =
      document->rootIndex >= 0
          ? (XMLElementNode *)document->nodes->nodes[document->rootIndex]
          : NULL;

  if (!root) {
    PRINT_ERROR("Missing root element for XMLDocument, cannot query\n");
    return nodes;
  }

  addNodeToCollection(nodes, (XMLNode *)root);

  NodeCollection *result = runQuery(nodes, query, true);

  freeNodeCollection(nodes);

  compactNodeCollection(result);

  return result;
}
