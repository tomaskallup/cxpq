#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

String *stringCreateEmpty() {
  struct String *str = malloc(sizeof(String));
  if (str == NULL) {
    PRINT_ERROR("Failed to allocate memory for string\n");
    return NULL;
  }

  str->value = NULL;
  str->length = 0;

  return str;
}

String *stringCreateFromChars(char *chars) {
  String *str = stringCreateEmpty();
  str->value = chars;
  str->length = strlen(chars);

  return str;
}

void stringAppendChar(String *str, char charToAdd) {
  str->length++;
  str->value = realloc(str->value, (1 + str->length) * sizeof(char));
  str->value[str->length - 1] = charToAdd;
  str->value[str->length] = '\0';
}

bool stringEqual(String *str, String *cmp) {
  return (str->length == cmp->length &&
          strncmp(str->value, cmp->value, str->length) == 0);
}

bool stringEqualChars(String *str, char *cmp) {
  return (str->length == strlen(cmp) &&
          strncmp(str->value, cmp, str->length) == 0);
}

void stringTrim(String *str) {
  if (str->value == NULL) return;
  int start = 0;

  while (isWhitespace(str->value[start]) && start < str->length)
    start++;

  int end = str->length - 1;
  while (end > 0 && isWhitespace(str->value[end]))
    end--;

  if (end <= start) {
    free(str->value);
    str->value = NULL;
    str->length = 0;

    return;
  }

  const int length = 1 + end - start;
  char *result = calloc(length + 1, sizeof(char));

  strncpy(result, &str->value[start], length);

  free(str->value);

  str->value = result;
}

String *stringSubstring(String *str, int start, int end) {
  if (start < 0)
    start = str->length + start;
  if (end < 0)
    end = str->length + end + 1;
  if (start > end || start < 0 || end > str->length) {
    PRINT_ERROR("Invalid substring range %i-%i, string length %i\n", start, end, str->length);
  }

  const int length = end - start;
  String *result = stringCreateEmpty();
  result->length = length;
  result->value = calloc(length + 1, sizeof(char));
  strncpy(result->value, &str->value[start], length);

  return result;
}
 
         
