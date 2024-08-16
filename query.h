#include <stdbool.h>
#include <stdlib.h>

#include "string.h"
#include "types.h"

#ifndef QUERY_H

#define QUERY_H

#define NESTING_NONE 0
#define NESTING_INDIRECT 0b01
#define NESTING_DIRECT 0b10
/** Can be used to check for any nesting (direct/indirect) */
#define NESTING_SOME NESTING_DIRECT | NESTING_INDIRECT

#define PRINT_QUERY(query)                                                     \
  printf("%i key: %s, value: %s, nesting: %hu\n", query->type,                   \
         query->key != NULL ? query->key->value : "(null)",                    \
         query->value != NULL ? query->value->value : "(null)",                \
         query->nesting)

enum QueryType {
  /** Used to query by element tag name, eg. `bookstore` */
  ElementName,
  /** Used to query by element attribute with optional value, eg. `name="Test"`
     or just `name` */
  ElementAttribute,
  /** Used to query by position in result, eg. `1` or `last()` or `len() - 1` */
  PositionSelector,
  /** Used to query by subelement value, eg. `price > 35.00` */
  SubElementValue,
  /** Used for custom selection by builtin functions, currently unused */
  FunctionFilter,
};

typedef struct Query {
  enum QueryType type;
  String *value;
  String *key;
  String *comparator;
  /** If `NESTING_DIRECT`, the query is direct descendant, thus only direct
   * children of parent query will be queried */
  unsigned short nesting;
  struct Query *next;
  struct Query *prev;
} Query;

Query *initQuery(enum QueryType type);

void freeQuery(Query *query);

#endif
