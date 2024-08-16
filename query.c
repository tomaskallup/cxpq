#include "query.h"

Query *initQuery(enum QueryType type) {
  Query *query = malloc(sizeof(Query));
  query->type = type;

  return query;
}

void freeQuery(Query *query) {
  if (query->value != NULL)
    free(query->value);

  if (query->next != NULL)
    freeQuery(query->next);

  free(query);
}
