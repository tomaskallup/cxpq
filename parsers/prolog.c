#include "prolog.h"

#include "../util.h"

bool parseProlog(FILE *file) {
  skipWhitespaces(file);

  // First find out if we have a start of prolog tag `<?`
  if (fgetc(file) == '<' && fgetc(file) == '?') {
    // Success
    char currentChar;
    char lastChar;
    bool inString = false;
    while ((currentChar = fgetc(file))) {
      if (currentChar == '\0' || currentChar == -1)
        break;
      // We found the end of prolog
      if (!inString && lastChar == '?' && currentChar == '>')
        return true;

      // Make sure we handle pesky attributes `<?xml version="1.1" test="?>"?>`
      if (currentChar == '"')
        inString = !inString;

      lastChar = currentChar;
    }

    printCurrentLineMarked(file);
    PRINT_ERROR("No closing tag `?>` for prolog\n");
    return false;
  } else {
    // Not prolog, move back
    fseek(file, -2, SEEK_CUR);
    PRINT_ERROR("Document should start with a prolog (<?xml ...?>)");

    return false;
  }
}
