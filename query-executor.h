#include <stdbool.h>

#include "types.h"
#include "query.h"
#include "node-collection.h"

#ifndef QUERY_EXECUTOR_H

#define QUERY_EXECUTOR_H

NodeCollection *executeQuery(XMLDocument *document, Query *query);

#endif
