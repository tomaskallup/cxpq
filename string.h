#include <stdbool.h>

#ifndef XML_STRING_H

#define XML_STRING_H

#define STRING_BUFFER_SIZE 10

typedef struct String {
  char *value;
  int allocated;
  int length;
} String;

String *stringCreateEmpty();

void stringAppendChar(String *str, char charToAdd);
void stringAppendChars(String *str, char *charsToAdd);
void stringAppend(String *str, String *strToAdd);

bool stringEqual(String *str, String *cmp);
bool stringEqualChars(String *str, char *cmp);

void stringTrim(String *str);
void freeString(String *str);

String *stringSubstring(String *str, int start, int end);

#endif
