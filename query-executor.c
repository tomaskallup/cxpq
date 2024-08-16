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
  if (query->prev == NULL && query->type != ElementName &&
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
    const bool hasValue = query->value != NULL && query->value->length > 0;

    if (isWildcard && !hasValue)
      return true;

    for (int i = 0; i < node->attributesSize; i++) {
      Attribute *attribute = node->attributes[i];
      if (isWildcard || stringEqual(attribute->name, query->key)) {
        if (!hasValue || stringEqual(attribute->content, query->value))
          return true;
        else return false;
      }
    }

    return false;
  }
  case SubElementValue: {
    if (node->children == NULL || node->children->size == 0)
      return false;

    const bool hasValue = query->value != NULL && query->value->length > 0;

    for (int i = 0; i < node->children->size; i++) {
      XMLNode *rawChild = node->children->nodes[i];
      if (rawChild->type != ELEMENT)
        continue;

      XMLElementNode *child = (XMLElementNode *)rawChild;
      if (child->children != NULL || child->children->size == 0)
        continue;

      if (stringEqual(child->tag, query->key)) {
        String *childTextContent = stringCreateEmpty();

        for (int j = 0; j < child->children->size; j++) {
          XMLNode *rawGrandChild = child->children->nodes[j];
          if (rawGrandChild->type != TEXT)
            continue;

          XMLTextNode *grandChild = (XMLTextNode *)rawGrandChild;
          stringAppend(childTextContent, grandChild->content);
        }

        if (!hasValue ||
            compare(childTextContent, query->comparator, query->value)) {
          freeString(childTextContent);
          return true;
        }

        freeString(childTextContent);
      }
    }
    return false;
  }
  case PositionSelector:
    return true;
  case FunctionFilter:
    return true;
  }

  return false;
}

NodeCollection *runQuery(NodeCollection *nodes, Query *query) {
  NodeCollection *result = initNodeCollection();

  if (nodes->size == 0)
    return result;

  Query *currentQuery = query;
  bool isDirect = query->nesting == NESTING_DIRECT;
  bool hasNesting = query->nesting & NESTING_SOME;
  /* PRINT_DEBUG("Running query %s (%s) on node %s\n", query->value->value,
              isDirect ? "true" : "false", node->tag->value); */

  for (int i = 0; i < nodes->size; i++) {
    const XMLNode *rawNode = nodes->nodes[i];
    if (rawNode->type != ELEMENT)
      continue;
    XMLElementNode *node = (XMLElementNode *)rawNode;

    if (checkNode(node, query))
      addNodeToCollection(result, (XMLNode *)node);

    // If we need to "descend", we check children
    if (!isDirect && hasNesting && node->children != NULL) {
      NodeCollection *childrenResult = runQuery(node->children, query);
      if (childrenResult->size > 0)
        concatNodeCollection(result, childrenResult);
      freeNodeCollection(childrenResult);
    }
  }

  if (result->size == 0)
    return result;

  Query *nextQuery = query->next;
  if (nextQuery != NULL) {
    if (nextQuery->nesting & NESTING_SOME) {
      NodeCollection *childrenNodes = initNodeCollection();
      for (int i = 0; i < result->size; i++) {
        XMLNode *node = result->nodes[i];
        if (node->type != ELEMENT)
          continue;
        XMLElementNode *elementNode = (XMLElementNode *)node;
        if (elementNode->children != NULL)
          concatNodeCollection(childrenNodes, elementNode->children);
      }

      freeNodeCollection(result);
      result = runQuery(childrenNodes, nextQuery);
      freeNodeCollection(childrenNodes);
    }

    NodeCollection *nextResult = runQuery(result, nextQuery);
    freeNodeCollection(result);
    result = nextResult;
  }

  return result;
}

NodeCollection *executeQuery(XMLDocument *document, Query *query) {
  NodeCollection *nodes = initNodeCollection();
  Query *currentQuery = query;

  XMLElementNode *root =
      document->rootIndex >= 0
          ? (XMLElementNode *)document->nodes->nodes[document->rootIndex]
          : NULL;

  if (root == NULL) {
    PRINT_ERROR("Missing root element for XMLDocument, cannot query\n");
    return nodes;
  }

  addNodeToCollection(nodes, (XMLNode *)root);

  NodeCollection *result = runQuery(nodes, query);

  freeNodeCollection(nodes);

  return result;
}
