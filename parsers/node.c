#include <ctype.h>
#include <stdlib.h>

#include "comment.h"
#include "node.h"

bool parseCloseNode(FILE *file, XMLNode *node) {
  char currentChar;
  XMLNode *lastChild;
  while ((currentChar = fgetc(file))) {
    if (currentChar == '>') {
      if (stringEqual(node->tag, node->closeTag)) {
        return true;
      }

      printCurrentLineMarked(file);
      PRINT_ERROR("Mismatched closing node, expected:  \"%s\", got \"%s\"\n",
                  node->tag->value, node->closeTag->value);
      return false;
    } else if (isValidNameChar(currentChar)) {
      stringAppendChar(node->closeTag, currentChar);
    } else {
      fseek(file, -1, SEEK_CUR);
      printCurrentLineMarked(file);
      PRINT_ERROR("Invalid character for closing node:  \"%c\"", currentChar);

      return false;
    }
  }

  printCurrentLineMarked(file);
  PRINT_ERROR("Missing `>` for closing node\n");
  return false;
}

bool parseNodeContent(FILE *file, XMLNode *node) {
  skipWhitespaces(file);

  char currentChar;
  XMLNode *lastChild = NULL;
  while ((currentChar = fgetc(file))) {
    if (currentChar == '<') {
      char nextChar = fgetc(file);
      if (nextChar == '/') {
        if (!parseCloseNode(file, node))
          return false;

        break;
      } else if (nextChar == '!') {
        for (int i = 0; i < 2; i++) {

          nextChar = fgetc(file);

          if (nextChar != '-') {
            printCurrentLineMarked(file);
            PRINT_ERROR("Unexpected character for start of comment \"%c\", "
                        "comments must start with `<!--`",
                        currentChar);
            return false;
          }
        }

        if (!parseComment(file))
          return false;
      } else {
        fseek(file, -1, SEEK_CUR);

        XMLNode *child = parseNode(file);

        if (child == NULL)
          return false;

        if (lastChild == NULL)
          node->child = child;
        else
          lastChild->sibling = child;

        lastChild = child;
      }
    } else {
      // Parse text content
      stringAppendChar(node->content, currentChar);
    }
  }

  stringTrim(node->content);

  return true;
}

bool parseAttributeValue(FILE *file, Attribute *attr) {
  char openQuote = fgetc(file);
  unsigned long openQuotePos = ftell(file);

  if (openQuote != '"' && openQuote != '\'') {
    printCurrentLineMarked(file);
    PRINT_ERROR("Missing opening quote for attribute\n");
    return false;
  }

  char currentChar;
  while ((currentChar = fgetc(file))) {
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

Attribute *parseAttribute(FILE *file) {
  skipWhitespaces(file);

  char firstChar = fgetc(file);
  fseek(file, -1, SEEK_CUR);
  if (firstChar == '>') {
    return NULL;
  }

  Attribute *attr = initAttribute();

  int nameLength = 0;

  char currentChar;
  while ((currentChar = fgetc(file))) {
    if (currentChar == '=') {
      if (parseAttributeValue(file, attr)) {
        return attr;
      }

      break;
    } else {
      if (currentChar == ':' && !attr->hasNamespace)
        attr->hasNamespace = true;
      else if (currentChar == ':') {
        printCurrentLineMarked(file);
        PRINT_ERROR("Attribute name contains a colon multiple times.\n");

        return NULL;
      }

      if (!isValidNameChar(currentChar)) {
        printCurrentLineMarked(file);
        PRINT_ERROR("Invalid character found for attribute name \"%c\"\n",
                    currentChar);

        return NULL;
      }

      attr->name[nameLength] = currentChar;
      attr->name[++nameLength] = '\0';
    }
  }

  return NULL;
}

XMLNode *parseNode(FILE *file) {
  const char firstChar = fgetc(file);
  if (!isalpha(firstChar) && firstChar != '_') {
    printCurrentLineMarked(file);
    PRINT_ERROR(
        "Elements must start with underscore or a letter, found \"%c\"\n",
        firstChar);

    return NULL;
  }

  XMLNode *node = initNode();
  stringAppendChar(node->tag, firstChar);

  bool parsingName = true;
  char currentChar;
  while ((currentChar = fgetc(file))) {
    if (currentChar != ' ' && currentChar != '>') {
      if (!parsingName) {
        printCurrentLineMarked(file);
        PRINT_ERROR("Invalid character found after element name \"%c\", "
                    "expected a space or `>`\n",
                    currentChar);

        return NULL;
      }
      if (!isValidNameChar(currentChar)) {
        printCurrentLineMarked(file);
        PRINT_ERROR("Invalid character found for element name \"%c\"\n",
                    currentChar);

        return NULL;
      }
      stringAppendChar(node->tag, currentChar);
    } else if (currentChar == '>') {
      // Parse node content
      if (!parseNodeContent(file, node)) {
        return NULL;
      }

      // We finished parsing this node
      break;
    } else if (currentChar == ' ') {
      parsingName = false;
      // Parse node attributes
      Attribute *attr = parseAttribute(file);

      if (attr == NULL)
        return NULL;

      // Append attributes, extending allocated memory
      node->attributesSize += 1;

      node->attributes =
          realloc(node->attributes, node->attributesSize * sizeof(Attribute *));
      node->attributes[node->attributesSize - 1] = attr;
    }
  }

  return node;
}
