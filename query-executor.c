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

enum CheckFunctionResultType {
  CFR_STRING,
  CFR_LONG,
  CFR_ULONG,
  CFR_FLOAT,
};

union CheckFunctionResultValue {
  String *str;
  long lng;
  size_t ulng;
  float flt;
};

struct CheckFunctionResult {
  enum CheckFunctionResultType type;
  bool dynamic;
  union CheckFunctionResultValue value;
};

struct NodeInfo {
  XMLElementNode *node;
  size_t index;
  size_t count;
};

float resultToFloat(struct CheckFunctionResult result) {
  switch (result.type) {
  case CFR_LONG:
    return (float)result.value.lng;
  case CFR_ULONG:
    return (float)result.value.ulng;
  case CFR_FLOAT:
    return result.value.flt;
  case CFR_STRING:
    return 0;
  }
}

String *extractNodeText(XMLElementNode *node) {
  String *result = stringCreateEmpty();

  if (node->children) {
    for (unsigned int i = 0; i <= node->children->lastIndex; i++) {
      XMLNode *rawNode = node->children->nodes[i];
      if (!rawNode || rawNode->type != TEXT)
        continue;
      XMLTextNode *node = (XMLTextNode *)rawNode;
      stringAppend(result, node->content);
    }
  }

  return result;
}

struct CheckFunctionResult checkFunction(String *value,
                                         struct NodeInfo nodeInfo) {
  struct CheckFunctionResult result = {0};
  result.dynamic = false;
  if (stringEqualChars(value, "position()")) {
    result.value.ulng = nodeInfo.index;
    result.type = CFR_ULONG;

    return result;
  } else if (stringEqualChars(value, "length()")) {
    result.value.ulng = nodeInfo.count;
    result.type = CFR_ULONG;

    return result;
  } else if (stringEqualChars(value, "text()")) {
    result.value.str = extractNodeText(nodeInfo.node);
    result.dynamic = true;
    result.type = CFR_STRING;
  } else {
    result.value.str = value;
    result.type = CFR_STRING;
  }

  if (result.type == CFR_STRING) {
    bool isNumeric = true;
    bool hasComma = false;

    String *strValue = result.value.str;

    for (int i = 0; i < strValue->length; i++) {
      char c = strValue->value[i];
      if (c == '.') {
        if (!hasComma)
          hasComma = true;
        else {
          isNumeric = false;
          break;
        }
      } else if (c < '0' && c > '9') {
        isNumeric = false;
        break;
      }
    }

    if (isNumeric && hasComma) {
      result.type = CFR_FLOAT;
      result.value.flt = (float)atof(value->value);
    } else if (isNumeric) {
      result.type = CFR_LONG;
      result.value.lng = atol(value->value);
    }
  }

  return result;
}

bool checkNode(struct NodeInfo nodeInfo, Query *query) {
  XMLElementNode *node = nodeInfo.node;
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

    for (unsigned int i = 0; i < node->attributesSize; i++) {
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
  case FunctionFilter: {
    struct CheckFunctionResult keyResult = checkFunction(query->key, nodeInfo);
    struct CheckFunctionResult valueResult =
        checkFunction(query->value, nodeInfo);
    bool retVal = false;

    if ((keyResult.type == CFR_STRING || valueResult.type == CFR_STRING) &&
        keyResult.type != valueResult.type) {
      // Cannot compare string & non-string
    } else {

      if (keyResult.type == CFR_STRING) {
        if (stringEqualChars(query->comparator, "=")) {
          retVal = stringEqual(keyResult.value.str, valueResult.value.str);
        }
        // No comparison for strings
      } else {
        // Ugly solution, we convert all to float so we can compare.
        float keyValue = resultToFloat(keyResult),
              valueValue = resultToFloat(valueResult);
        if (stringEqualChars(query->comparator, ">")) {
          retVal = keyValue > valueValue;
        } else if (stringEqualChars(query->comparator, ">=")) {
          retVal = keyValue >= valueValue;
        } else if (stringEqualChars(query->comparator, "<")) {
          retVal = keyValue < valueValue;
        } else if (stringEqualChars(query->comparator, "<=")) {
          retVal = keyValue <= valueValue;
        } else if (stringEqualChars(query->comparator, "=")) {
          retVal = keyValue == valueValue;
        }
      }
    }

    if (keyResult.dynamic) {
      if (keyResult.type == CFR_STRING)
        freeString(keyResult.value.str);
    }
    if (valueResult.dynamic) {
      if (valueResult.type == CFR_STRING)
        freeString(valueResult.value.str);
    }
    return retVal;
  }
  }

  return false;
}

// <book lang="cs"><book lang="en">test</book></book>
// `/book` => <book lang="cs">
// `//book` => <book lang="cs">,<book lang="en">
// `//book[@lang=en]` => <book lang="en">
// `//book[@lang=cs]` => <book lang="cs">

NodeCollection *runQuery(NodeCollection *nodes, Query *query,
                         bool checkNodeDirectly, bool checkNext,
                         long indexOverride) {
  NodeCollection *result = initNodeCollection();

  if (nodes->size == 0)
    return result;

  bool hasNesting = query->nesting & NESTING_SOME;
  bool isDirect = query->nesting == NESTING_DIRECT;

  for (unsigned int i = 0; i <= nodes->lastIndex; i++) {
    XMLNode *rawNode = nodes->nodes[i];
    if (!rawNode || rawNode->type != ELEMENT)
      continue;
    XMLElementNode *node = (XMLElementNode *)rawNode;

    struct NodeInfo nodeInfo = {.node = node,
                                .index = indexOverride >= 0 ? indexOverride : i,
                                .count = nodes->size};

    if (checkNodeDirectly && checkNode(nodeInfo, query))
      addNodeToCollection(result, rawNode);

    // If we need to "descend", we check children
    if (hasNesting && !isDirect && node->children) {
      NodeCollection *childrenResult =
          runQuery(node->children, query, true, false, -1);
      if (childrenResult->size > 0) {
        concatNodeCollection(result, childrenResult);
      }
      freeNodeCollection(childrenResult);
    }
  }

  if (result->size == 0)
    return result;

  if (query->subQueryCount > 0) {
    for (unsigned int i = 0; i < query->subQueryCount; i++) {
      Query *subQuery = query->subQueries[i];
      NodeCollection *resultCopy = cloneNodeCollection(result);
      for (unsigned int i = 0; i <= resultCopy->lastIndex; i++) {
        XMLNode *currentNode = resultCopy->nodes[i];

        if (!currentNode)
          continue;

        NodeCollection elementOnly = {
            .size = 1,
            .allocated = 1,
            .lastIndex = 0,
            .nodes = &currentNode,
        };
        NodeCollection *newResult = runQuery(
            &elementOnly, subQuery,
            // Subquery should be executed on children if it's ElementName,
            // otherwise it should directly check the node itself
            subQuery->type != ElementName, true, i);

        if (newResult->size == 0)
          removeNodeFromCollection(resultCopy, currentNode, true);
        freeNodeCollection(newResult);
      }

      freeNodeCollection(result);
      result = resultCopy;
    }
  }

  if (checkNext) {
    Query *nextQuery = query->next;
    if (nextQuery) {
      const bool nextHasNesting = nextQuery->nesting & NESTING_SOME;
      if (nextHasNesting) {
        NodeCollection *newResult = initNodeCollection();
        for (unsigned int i = 0; i <= result->lastIndex; i++) {
          XMLNode *node = result->nodes[i];
          if (!node || node->type != ELEMENT)
            continue;
          XMLElementNode *elementNode = (XMLElementNode *)node;
          if (elementNode->children) {
            NodeCollection *childrenResult =
                runQuery(elementNode->children, nextQuery, true, true, -1);
            concatNodeCollection(newResult, childrenResult);
            freeNodeCollection(childrenResult);
          }
        }

        freeNodeCollection(result);
        result = newResult;
      } else {
        NodeCollection *newResult = runQuery(result, nextQuery, true, true, -1);

        freeNodeCollection(result);
        result = newResult;
      }
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

  NodeCollection *result = runQuery(nodes, query, true, true, -1);

  freeNodeCollection(nodes);

  compactNodeCollection(result);

  return result;
}
