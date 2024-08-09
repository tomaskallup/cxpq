#include <ctype.h>
#include <string.h>

#include "../util.h"
#include "dtd.h"

const char *dtdTagName = "DOCTYPE";

bool parseDTDLiteral(FILE *file) { return true; }

XMLDTDNode *parseDTD(FILE *file) {
  /* DTD Must start with DOCTYPE */
  for (int i = 0; i < 7; i++) {
    char currentChar = fgetc(file);

    if (!currentChar) {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Unexpected end of file inside DTD tag name\n");

      return NULL;
    } else if (currentChar != dtdTagName[i]) {
      fseek(file, -i, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Invalid DTD tag name, expected DOCTYPE\n");

      return NULL;
    }
  }

  if (fgetc(file) != ' ') {
    fseek(file, -1, SEEK_CUR);
    printCurrentLineMarked(file);
    PRINT_ERROR("Missing space after DOCTYPE tag name\n");
    return NULL;
  }

  /* Next is name of the DTD */
  const char firstChar = fgetc(file);
  if (!isalpha(firstChar) && firstChar != '_') {
    fseek(file, -1, SEEK_CUR);
    printCurrentLineMarked(file);
    PRINT_ERROR(
        "DTD name must start with underscore or a letter, found \"%c\"\n",
        firstChar);

    return NULL;
  }

  char currentChar;
  while ((currentChar = fgetc(file))) {
    if (currentChar == ' ' || currentChar == '>')
      break;
    if (!isValidNameChar(currentChar)) {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("DTD name contains invalid character \"%c\"\n", currentChar);

      return NULL;
    }
  }

  fseek(file, -1, SEEK_CUR);

  XMLDTDNode *node = (XMLDTDNode *)initNode(DTD);

  char nextChar = fgetc(file);
  if (nextChar == '>')
    return node;

  if (nextChar != ' ') {
    fseek(file, -1, SEEK_CUR);
    printCurrentLineMarked(file);
    PRINT_ERROR("Missing space after DTD name\n");

    freeXMLTree((XMLNode *)node);
    return NULL;
  }

  nextChar = fgetc(file);
  if (nextChar == '[') {
    while ((nextChar = fgetc(file)) != ']') {
      stringAppendChar(node->content, nextChar);
    }

    if (fgetc(file) != '>') {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Missing `>` after internal DTD content\n");

      freeXMLTree((XMLNode *)node);
      return NULL;
    }

  } else {
    char externalID[6] = {nextChar};
    for (int i = 1; i < 6; i++) {
      externalID[i] = fgetc(file);
    }

    bool isPublic = strncmp(externalID, "PUBLIC", 6) == 0;
    bool isSystem = strncmp(externalID, "SYSTEM", 6) == 0;
    if (isPublic || isSystem) {
      if (fgetc(file) != ' ') {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Missing space after DTD external ID\n");
        freeXMLTree((XMLNode *)node);
        return NULL;
      }

      if (!parseDTDLiteral(file)) {
        freeXMLTree((XMLNode *)node);
        return NULL;
      }

      if (isPublic) {
        if (fgetc(file) != ' ') {
          fseek(file, -1, SEEK_CUR);
          printCurrentLineMarked(file);
          PRINT_ERROR("Missing space after DTD PUBLIC ID literal\n");
          freeXMLTree((XMLNode *)node);
          return NULL;
        }
      }

      if (!parseDTDLiteral(file)) {
        freeXMLTree((XMLNode *)node);
        return NULL;
      }

      if (fgetc(file) != '>') {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Expected `>` after DTD literal\n");
        freeXMLTree((XMLNode *)node);
        return NULL;
      }
    } else {
      fseek(file, -6, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Invalid DTD external ID, expected \"SYSTEM\" or \"PUBLIC\", "
                  "got \"%s\"\n",
                  externalID);
      freeXMLTree((XMLNode *)node);
      return NULL;
    }
  }

  return node;
}
