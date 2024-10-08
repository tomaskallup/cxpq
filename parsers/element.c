#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "cdata.h"
#include "comment.h"
#include "element.h"
#include "node.h"

#include "../node-collection.h"

bool parseCloseTag(FILE *file, XMLElementNode *node) {
  char currentChar;
  while ((currentChar = (char)fgetc(file))) {
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
      PRINT_ERROR("Invalid character for closing node:  \"%c\"\n", currentChar);

      return false;
    }
  }

  printCurrentLineMarked(file);
  PRINT_ERROR("Missing `>` for closing node\n");
  return false;
}

bool parseElementContent(FILE *file, XMLElementNode *node) {
  skipWhitespaces(file);

  char currentChar;
  XMLNode *lastChild = NULL;
  while ((currentChar = (char)fgetc(file))) {
    if (currentChar == '<') {
      char nextChar = (char)fgetc(file);
      if (nextChar == '/') {
        if (!parseCloseTag(file, node))
          return false;

        break;
      } else if (nextChar == '!') {
        nextChar = (char)fgetc(file);
        XMLNode *resultNode = NULL;
        if (nextChar == '-') {
          nextChar = (char)fgetc(file);

          if (nextChar != '-') {
            fseek(file, -1, SEEK_CUR);
            printCurrentLineMarked(file);
            PRINT_ERROR(
                "Unexpected character for start of comment or CDATA \"%c\", "
                "comments must start with `<!--`, CDATA must start with "
                "`<[!CDATA[`\n",
                currentChar);
            return false;
          }

          resultNode = (XMLNode *)parseComment(file);
        } else if (nextChar == '[') {
          resultNode = (XMLNode *)parseCDATA(file);
        }

        if (!resultNode)
          return false;

        addNodeToCollection(node->children, resultNode);

        if (lastChild && lastChild->type == TEXT) {
          XMLTextNode *textNode = (XMLTextNode *)lastChild;
          stringTrim(textNode->content);
        }
        lastChild = resultNode;
      } else {
        fseek(file, -1, SEEK_CUR);

        XMLNode *child = parseNode(file, (XMLNode *)node);

        if (!child)
          return false;

        addNodeToCollection(node->children, child);

        if (lastChild && lastChild->type == TEXT) {
          XMLTextNode *textNode = (XMLTextNode *)lastChild;
          stringTrim(textNode->content);
        }
        lastChild = child;
      }
    } else {
      if (!lastChild ||
          (lastChild->type != TEXT && !isWhitespace(currentChar))) {
        XMLNode *textNode = initNode(TEXT);

        addNodeToCollection(node->children, textNode);

        lastChild = textNode;
      }
      if (lastChild && lastChild->type == TEXT) {
        // Parse text content
        XMLTextNode *textNode = (XMLTextNode *)lastChild;
        stringAppendChar(textNode->content, currentChar);
      }
    }
  }

  if (lastChild && lastChild->type == TEXT) {
    XMLTextNode *textNode = (XMLTextNode *)lastChild;
    stringTrim(textNode->content);
  }

  return true;
}

bool parseAttributeValue(FILE *file, Attribute *attr) {
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

Attribute *parseAttribute(FILE *file) {
  skipWhitespaces(file);

  char firstChar = (char)fgetc(file);
  fseek(file, -1, SEEK_CUR);
  if (firstChar == '>') {
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

XMLElementNode *parseElement(FILE *file) {
  const char firstChar = (char)fgetc(file);
  if (!isalpha(firstChar) && firstChar != '_') {
    fseek(file, -1, SEEK_CUR);
    printCurrentLineMarked(file);
    PRINT_ERROR(
        "Elements must start with underscore or a letter, found \"%c\"\n",
        firstChar);

    return NULL;
  }

  XMLElementNode *node = (XMLElementNode *)initNode(ELEMENT);
  stringAppendChar(node->tag, firstChar);

  bool parsingName = true;
  char currentChar;
  while ((currentChar = (char)fgetc(file))) {
    if (currentChar != ' ' && currentChar != '>') {
      if (!parsingName) {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Invalid character found after element name \"%c\", "
                    "expected a space or `>`\n",
                    currentChar);

        freeXMLNode((XMLNode *)node);
        return NULL;
      }
      if (!isValidNameChar(currentChar)) {
        fseek(file, -1, SEEK_CUR);
        printCurrentLineMarked(file);
        PRINT_ERROR("Invalid character found for element name \"%c\"\n",
                    currentChar);

        freeXMLNode((XMLNode *)node);
        return NULL;
      }
      stringAppendChar(node->tag, currentChar);
    } else if (currentChar == '>') {
      // Parse node content
      if (!parseElementContent(file, node)) {
        freeXMLNode((XMLNode *)node);
        return NULL;
      }

      // We finished parsing this node
      break;
    } else if (currentChar == ' ') {
      parsingName = false;
      // Parse node attributes
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

  return node;
}
