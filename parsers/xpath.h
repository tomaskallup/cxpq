#include "../query.h"

Query *parseXpath(const char *rawQuery);

typedef struct ParserCharCurosr {
  const char *value;
  size_t pos;
} ParserCharCurosr;
