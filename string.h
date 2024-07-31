#include <stdbool.h>

#ifndef XML_STRING_H

#define XML_STRING_H

typedef struct String {
  char *value;
  int length;
} String;

String *stringCreateEmpty();

String *stringCreateFromChars(char *chars);

void stringAppendChar(String *str, char charToAdd);

bool stringEqual(String *str, String *cmp);
bool stringEqualChars(String *str, char *cmp);

void stringTrim(String *str);

String *stringSubstring(String *str, int start, int end);

#endif
