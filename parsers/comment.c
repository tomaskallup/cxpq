#include "comment.h"

#include "../util.h"

bool parseComment(FILE *file) {
  char firstChar = fgetc(file);

  if (firstChar == '-') {
    printCurrentLineMarked(file);
    PRINT_ERROR("Comments cannot start with `-`\n");
    return false;
  }

  String *content = stringCreateEmpty();
  stringAppendChar(content, firstChar);

  char currentChar;
  while ((currentChar = fgetc(file))) {
    stringAppendChar(content, currentChar);

    if (currentChar == '>') {
      String *end = stringSubstring(content, -3, -1);
      if (stringEqualChars(end, "-->")) {

        freeString(end);
        freeString(content);
        return true;
      }

      freeString(end);
    }
  }

  printCurrentLineMarked(file);
  PRINT_ERROR("Missing closing tag for comment\n");
  freeString(content);
  return false;
}
