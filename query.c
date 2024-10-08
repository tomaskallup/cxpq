#include <stdio.h>
#include <stdlib.h>

#include "query.h"

Query *initQuery(enum QueryType type) {
  Query *query = malloc(sizeof(Query));
  query->type = type;
  query->key = NULL;
  query->value = NULL;
  query->next = NULL;
  query->prev = NULL;
  query->comparator = NULL;
  query->subQueries = NULL;
  query->subQueryCount = 0;

  return query;
}

void freeQuery(Query *query) {
  if (query->value)
    freeString(query->value);

  if (query->key)
    freeString(query->key);

  if (query->comparator)
    freeString(query->comparator);

  if (query->next)
    freeQuery(query->next);

  if (query->subQueries) {
    for (unsigned int i = 0; i < query->subQueryCount; i++)
      freeQuery(query->subQueries[i]);
    free(query->subQueries);
  }

  free(query);
}

void printQuery(Query *query, unsigned int depth, bool printNext) {
  for (unsigned int i = 0; i < depth; i++)
    fprintf(stderr, "  ");
  if (query->type == ElementName) {
    if (query->nesting == NESTING_DIRECT)
      printf("/");
    else if (query->nesting == NESTING_INDIRECT)
      printf("//");
  } else if (query->type == ElementAttribute)
    printf("@");
  else if (query->type == FunctionFilter)
    printf("F:");

  if (query->key)
    printf("%s", query->key->value);
  if (query->comparator)
    printf("%s", query->comparator->value);
  if (query->value)
    printf("%s", query->value->value);

  printf("\n");

  if (query->subQueries)
    for (unsigned int i = 0; i < query->subQueryCount; i++)
      printQuery(query->subQueries[i], depth + 1, printNext);

  if (printNext && query->next)
    printQuery(query->next, depth, printNext);
}

bool queryIsEmpty(Query *query) {
  if (query->type == ElementName)
    return !query->value;
  if (query->type == ElementAttribute)
    return !query->key;
  if (query->type == FunctionFilter)
    return !query->key || !query->comparator || !query->value;

  return false;
}
