#include <assert.h>
#include <stdlib.h>

#include "processing-instruction.h"

#include "../util.h"

static bool parseAttributeValue(FILE *file, Attribute *attr) {
  char openQuote = (char)fgetc(file);
  long openQuotePos = ftell(file);

  if (openQuote != '"' && openQuote != '\'') {
    printCurrentLineMarked(file);
    PRINT_ERROR("Missing opening quote for attribute\n");
    return false;
  }

  char currentChar;
  while ((currentChar = (char)fgetc(file))) {
    if (currentChar == openQuote) {
      return true;
    } else {
      stringAppendChar(attr->content, currentChar);
    }
  }

  fseek(file, openQuotePos, SEEK_SET);
  printCurrentLineMarked(file);
  PRINT_ERROR("Missing closing quote for attribute\n");
  return false;
}

static Attribute *parseAttribute(FILE *file) {
  skipWhitespaces(file);

  char firstChar = (char)fgetc(file);
  char secondChar = (char)fgetc(file);
  fseek(file, -2, SEEK_CUR);
  if (firstChar == '?' && secondChar == '>') {
    return NULL;
  }

  Attribute *attr = initAttribute();

  char currentChar;
  while ((currentChar = (char)fgetc(file))) {
    if (currentChar == '=') {
      if (parseAttributeValue(file, attr)) {
        return attr;
      }

      break;
    } else {
      if (currentChar == ':' && !attr->hasNamespace) {
        attr->hasNamespace = true;
      } else if (currentChar == ':') {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Attribute name contains a colon multiple times.\n");

        freeAttribute(attr);
        return NULL;
      }

      if (!isValidNameChar(currentChar)) {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Invalid character found for attribute name \"%c\"\n",
                    currentChar);

        freeAttribute(attr);
        return NULL;
      }

      stringAppendChar(attr->name, currentChar);
    }
  }

  freeAttribute(attr);
  return NULL;
}

XMLNode *parseProcessingInstruction(FILE *file) {
  skipWhitespaces(file);

  XMLProcessingInstructionNode *node =
      (XMLProcessingInstructionNode *)initNode(PROCESSING_INSTRUCTION);

  // Success
  char currentChar;
  bool parsingName = true;
  while ((currentChar = (char)fgetc(file))) {
    if (parsingName) {
      if (isValidNameChar(currentChar)) {
        stringAppendChar(node->tag, currentChar);
      } else if (isWhitespace(currentChar)) {
        parsingName = false;
      } else {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR(
            "Invalid character found for processing instruction name: \"%c\"\n",
            currentChar);
        freeXMLNode((XMLNode *)node);
        return NULL;
      }
    } else {
      if (currentChar == '\0' || currentChar == -1)
        break;
      // We found the end of instructin
      if (currentChar == '?' && (char)fgetc(file) == '>')
        return (XMLNode *)node;
      else if (currentChar == '?') {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Invalid character found after processing instruction "
                    "name: \"%c\"\n",
                    currentChar);
        freeXMLNode((XMLNode *)node);
        return NULL;
      }

      fseek(file, -1, SEEK_CUR);

      Attribute *attr = parseAttribute(file);

      if (!attr) {
        freeXMLNode((XMLNode *)node);
        return NULL;
      }

      // Append attributes, extending allocated memory
      node->attributesSize += 1;

      Attribute **newAttributes =
          realloc(node->attributes, node->attributesSize * sizeof(Attribute *));
      assert(newAttributes && "Failed to allocate new attributes for node");

      node->attributes = newAttributes;
      node->attributes[node->attributesSize - 1] = attr;
    }
  }

  freeXMLNode((XMLNode *)node);

  printCurrentLineMarked(file);
  PRINT_ERROR("No closing tag `?>` for prolog\n");
  return NULL;
}
