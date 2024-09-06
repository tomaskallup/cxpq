#include <ctype.h>

#include "xpath.h"

#include "../util.h"

char nextToken(ParserCharCurosr *cursor) {
  if (cursor->value[cursor->pos] == '\0')
    return '\0';

  return cursor->value[cursor->pos++];
}

void finalizeQuery(Query *query) {
  if (query->type == ElementName && !query->value &&
      query->key) {
    query->value = query->key;
    query->key = NULL;
  }
}

Query *parseXpathQuery(ParserCharCurosr *cursor, bool isTopLevel) {
  Query *query = initQuery(ElementName);
  Query *firstQuery = query;
  bool isFirstItem = true;
  bool isRelative = false;

  char currentChar, lastChar = '\0';

  while ((currentChar = nextToken(cursor)) != '\0') {
    if (currentChar != '/') {
      if (lastChar == '/') {
        // Init new query for next level
        if (query->key) {
          finalizeQuery(query);
          Query *newQuery = initQuery(ElementName);
          isFirstItem = false;
          query->next = newQuery;
          newQuery->prev = query;
          query = newQuery;
        }

        if (isRelative)
          query->nesting = NESTING_INDIRECT;
        else
          query->nesting = NESTING_DIRECT;
      }

      if (!query && isFirstItem && isTopLevel) {
        PRINT_ERROR("Expected \"/\" to start xpath selector\n");
        goto parsing_failed;
      }
    }

    switch (currentChar) {
    case '/': {
      if (lastChar != '/')
        isRelative = false;
      else if (!isRelative)
        isRelative = true;
      else {
        PRINT_ERROR("Found unexpected \"/\" in xpath query, there are already "
                    "two before this one.\n");
        goto parsing_failed;
      }
    } break;
    case '@': {
      if (isTopLevel) {
        PRINT_ERROR("Attribute query \"@\" is only allowed in sub queries "
                    "(inside \"[]\").\n");
        goto parsing_failed;
      }
      // Parse attribute query
      if (query->value) {
        PRINT_ERROR("Found unexpected \"@\" inside query, attribute query "
                    "should always be first in sub query (\"[@...]\")\n");
        goto parsing_failed;
      }

      query->type = ElementAttribute;
    } break;
    case '[': {
      const size_t index = query->subQueryCount;
      query->subQueryCount++;
      query->subQueries =
          realloc(query->subQueries, query->subQueryCount * sizeof(Query *));

      Query *newSubQuery = parseXpathQuery(cursor, false);

      if (!newSubQuery)
        goto parsing_failed;

      query->subQueries[index] = newSubQuery;
    } break;
    case ']': {
      if (isTopLevel) {
        PRINT_ERROR("Found unexpected \"]\" in xpath query, there was no "
                    "matching \"[\".\n");
        goto parsing_failed;
      }

      if (!firstQuery->key) {
        PRINT_ERROR("Found empty subQuery, empty \"[]\" is is not allowed.");
        goto parsing_failed;
      }

      goto finish;
    } break;
    case ' ': {
      // Skip?
    } break;
    case '(': {
      if (isTopLevel) {
        PRINT_ERROR("Found unexpected \"(\" in xpath query, functions are "
                    "currently not allowed in top level.\n");
        goto parsing_failed;
      }

      if (query->type != FunctionFilter)
        query->type = FunctionFilter;

      goto append_to_query;
    } break;
    case ')': {
      goto append_to_query;
    } break;
    case '=':
    case '<':
    case '>': {
      if (query->value) {
        PRINT_ERROR("Found unexpected \"%c\" in xpath query, this query "
                    "already has a value, comparator should be before it.\n",
                    currentChar);
        goto parsing_failed;
      }
      if (!query->comparator) {
        query->comparator = stringCreateEmpty();
        stringAppendChar(query->comparator, currentChar);
      } else {
        if (currentChar == '=' && query->comparator->value[0] != '=' &&
            query->comparator->length < 2) {
          stringAppendChar(query->comparator, currentChar);
        } else {
          PRINT_ERROR("Found unexpected \"%c\" in xpath query, this query "
                      "already has a complete comparator \"%s\".\n",
                      currentChar, query->comparator->value);
          goto parsing_failed;
        }
      }
    } break;
    default:
    append_to_query:
      if (!query->comparator) {
        if (!query->key)
          query->key = stringCreateEmpty();

        stringAppendChar(query->key, currentChar);
      } else {
        if (!query->value)
          query->value = stringCreateEmpty();

        stringAppendChar(query->value, currentChar);
      }

      break;
    }
    lastChar = currentChar;
  }

  if (!isTopLevel) {
    PRINT_ERROR("Missing \"]\" to close subQuery.");
    goto parsing_failed;
  }

finish:
  finalizeQuery(query);
  query = firstQuery;
  return query;

parsing_failed:
  PRINT_ERROR("Failed to parse XPath query\n");
  freeQuery(query);
  return NULL;
}

Query *parseXpath(const char *rawQuery) {
  ParserCharCurosr cursor = {
      .value = rawQuery,
      .pos = 0,
  };

  return parseXpathQuery(&cursor, true);
}
