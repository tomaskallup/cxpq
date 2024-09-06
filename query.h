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

enum QueryType {
  /** Used to query by element tag name, eg. `bookstore` */
  ElementName,
  /** Used to query by element attribute with optional value, eg. `name="Test"`
     or just `name` */
  ElementAttribute,
  /** Used for custom selection by builtin functions
   * eg.:
   *   `last()`
   *   `len() - 1`
   *   `position() > 1`
   *   `text() > 1`
   *   `> 1` -> `text() > 1`
   *   `1` -> `position() = 1`
   * */
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
  /** These queries are run on matching nodes as "root", if the have any results
   * the "root" node is considered matching
   */
  struct Query **subQueries;
  size_t subQueryCount;
} Query;

Query *initQuery(enum QueryType type);

void freeQuery(Query *query);
void printQuery(Query *query, size_t depth);

#endif
