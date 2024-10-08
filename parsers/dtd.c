#include <ctype.h>
#include <string.h>

#include "../util.h"
#include "dtd.h"

const char *dtdTagName = "DOCTYPE";

static bool parseDTDLiteral(FILE *file, String *literal) {
  const char firstChar = (char)fgetc(file);
  if (firstChar != '"') {
    fseek(file, -1, SEEK_CUR);
    printCurrentLineMarked(file);
    PRINT_ERROR("Missing \" to start DTD Literal, got \"%c\"\n", firstChar);

    return false;
  }

  char currentChar;
  while ((currentChar = (char)fgetc(file))) {
    if (currentChar == '"')
      return true;
    stringAppendChar(literal, currentChar);
  }

  fseek(file, -1, SEEK_CUR);
  printCurrentLineMarked(file);
  PRINT_ERROR("Missing \" to end DTD Literal, got \"%c\"\n", firstChar);
  return false;
}

XMLDTDNode *parseDTD(FILE *file) {
  /* DTD Must start with DOCTYPE */
  for (unsigned short i = 0; i < 7; i++) {
    char currentChar = (char)fgetc(file);

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

  XMLDTDNode *node = (XMLDTDNode *)initNode(DTD);

  /* Next is name of the DTD */
  const char firstChar = (char)fgetc(file);
  if (!isalpha(firstChar) && firstChar != '_') {
    fseek(file, -1, SEEK_CUR);
    printCurrentLineMarked(file);
    PRINT_ERROR(
        "DTD name must start with underscore or a letter, found \"%c\"\n",
        firstChar);

    return NULL;
  }

  stringAppendChar(node->name, firstChar);
  char currentChar;
  while ((currentChar = (char)fgetc(file))) {
    if (currentChar == ' ' || currentChar == '>')
      break;
    if (!isValidNameChar(currentChar)) {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("DTD name contains invalid character \"%c\"\n", currentChar);

      return NULL;
    }
    stringAppendChar(node->name, currentChar);
  }

  fseek(file, -1, SEEK_CUR);

  char nextChar = (char)fgetc(file);
  if (nextChar == '>')
    return node;

  if (nextChar != ' ') {
    fseek(file, -1, SEEK_CUR);
    printCurrentLineMarked(file);
    PRINT_ERROR("Missing space after DTD name\n");

    freeXMLNode((XMLNode *)node);
    return NULL;
  }

  nextChar = (char)fgetc(file);
  if (nextChar == '[') {
    node->content = stringCreateEmpty();
    while ((nextChar = (char)fgetc(file)) != ']') {
      stringAppendChar(node->content, nextChar);
    }

    if (fgetc(file) != '>') {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Missing `>` after internal DTD content\n");

      freeXMLNode((XMLNode *)node);
      return NULL;
    }

  } else {
    char externalID[6] = {nextChar};
    for (unsigned short i = 1; i < 6; i++) {
      externalID[i] = (char)fgetc(file);
    }

    bool isSystem = strncmp(externalID, "SYSTEM", 6) == 0;
    if (isSystem) {
      node->systemID = stringCreateEmpty();
      if (fgetc(file) != ' ') {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Missing space after DTD external ID\n");
        freeXMLNode((XMLNode *)node);
        return NULL;
      }

      if (!parseDTDLiteral(file, node->systemID)) {
        freeXMLNode((XMLNode *)node);
        return NULL;
      }

      if (fgetc(file) != '>') {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Expected `>` after DTD literal\n");
        freeXMLNode((XMLNode *)node);
        return NULL;
      }
    } else {
      fseek(file, -6, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Invalid DTD external ID, expected \"SYSTEM\", "
                  "got \"%s\"\n",
                  externalID);
      freeXMLNode((XMLNode *)node);
      return NULL;
    }
  }

  return node;
}
