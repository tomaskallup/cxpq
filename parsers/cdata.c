#include <ctype.h>
#include <string.h>

#include "../util.h"
#include "dtd.h"

const char *CDATATagName = "CDATA";
const char *CDATAClosing = "]]>";

XMLTextNode *parseCDATA(FILE *file) {
  /* DTD Must start with DOCTYPE */
  for (int i = 0; i < 5; i++) {
    char currentChar = fgetc(file);

    if (!currentChar) {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Unexpected end of file inside CDATA tag name\n");

      return NULL;
    } else if (currentChar != CDATATagName[i]) {
      fseek(file, -i, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Invalid CDATA tag name, expected CDATA\n");

      return NULL;
    }
  }

  if (fgetc(file) != '[') {
    fseek(file, -1, SEEK_CUR);
    printCurrentLineMarked(file);
    PRINT_ERROR("Missing opening bracket `[` after CDATA tag name\n");
    return NULL;
  }

  XMLTextNode *node = (XMLTextNode *)initNode(TEXT);
  node->isCDATA = true;

  /* Read content of CDATA */
  char currentChar;
  while ((currentChar = fgetc(file)) != CDATAClosing[0]) {
    stringAppendChar(node->content, currentChar);
  }

  for (int i = 1; i < strlen(CDATAClosing); i++) {
    if (fgetc(file) != CDATAClosing[i]) {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Invalid character for CDATA closing tag, expecting %c\n",
                  CDATAClosing[i]);
      freeXMLTree((XMLNode *)node);
      return NULL;
    }
  }

  return node;
}
