#include "prolog.h"

#include "../util.h"

XMLNode *parseProlog(FILE *file) {
  skipWhitespaces(file);

  XMLNode *node = initNode(PROCESSING_INSTRUCTION);

  // Success
  char currentChar;
  char lastChar;
  bool inString = false;
  while ((currentChar = fgetc(file))) {
    if (currentChar == '\0' || currentChar == -1)
      break;
    // We found the end of prolog
    if (!inString && lastChar == '?' && currentChar == '>')
      return node;

    // Make sure we handle pesky attributes `<?xml version="1.1" test="?>"?>`
    if (currentChar == '"')
      inString = !inString;

    lastChar = currentChar;
  }

  freeXMLTree(node);

  printCurrentLineMarked(file);
  PRINT_ERROR("No closing tag `?>` for prolog\n");
  return NULL;
}
