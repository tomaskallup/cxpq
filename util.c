#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include "util.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

bool isWhitespace(const char input) {
  return (input == ' ' || input == '\t' || input == '\n');
}

bool isValidNameChar(const char input) {
  if (isalnum(input) || input == ':' || input == '-' || input == '_' ||
      input == '.')
    return true;

  return false;
}

void skipWhitespaces(FILE *file) {
  char currentChar;
  while ((currentChar = fgetc(file))) {
    if (!isWhitespace(currentChar))
      break;
  }

  // Move back
  fseek(file, -1, SEEK_CUR);
}

void printCurrentLineMarked(FILE *file) {
  unsigned long position = ftell(file);

  fseek(file, 0, SEEK_SET);

  unsigned long lineNumber = 1;
  unsigned long column = 1;

  // printf("%lu: ", lineNumber);
  while (ftell(file) < position - 1) {
    const char currentChar = fgetc(file);
    // printf("%c", currentChar);
    if (currentChar == '\n') {
      lineNumber++;
      column = 1;
      // printf("%lu: ", lineNumber);
    }

    column++;
  }

  printf("\n");

  unsigned long lineLength = column;
  while ((lineLength - column) < ERROR_MAX_LENGTH && fgetc(file) != '\n')
    lineLength++;

  fseek(file, column - lineLength - 1, SEEK_CUR);

  unsigned long start =
      column > ERROR_MAX_LENGTH ? column - ERROR_MAX_LENGTH : 0;
  unsigned long end = start + ERROR_MAX_LENGTH;
  if (end >= lineLength)
    end = lineLength - 1;
  unsigned long offset = column - start - 1;

  char *offendingLine = calloc(column, sizeof(char));

  /* DEBUG_PRINT("Start %lu end %lu column %lu rewind %lu lineLength %lu\n",
     start, end, column, offset, lineLength); */

  fseek(file, -offset, SEEK_CUR);

  fprintf(stderr, "At line %lu, column %lu\n", lineNumber, column);

  for (int i = 0; i < end - start; i++) {
    const char character = fgetc(file);
    fprintf(stderr, "%c", character);
  }
  fprintf(stderr, "\n");
  for (int i = 0; i < offset; i++)
    fprintf(stderr, " ");
  fprintf(stderr, "^\n");
}

XMLNode *initNode() {
  XMLNode *node = malloc(sizeof(XMLNode));

  node->content = stringCreateEmpty();
  node->attributes = NULL;
  node->attributesSize = 0;
  node->tag = stringCreateEmpty();
  node->closeTag = stringCreateEmpty();
  node->child = NULL;
  node->sibling = NULL;
  node->parent = NULL;

  return node;
}

Attribute *initAttribute() {
  Attribute *attribute = malloc(sizeof(Attribute));

  attribute->name[0] = '\0';
  attribute->content = stringCreateEmpty();

  return attribute;
}

void freeString(String *str) {
  if (str->value != NULL) {
    free(str->value);
  }

  free(str);
}

void freeAttribute(Attribute *attribute) {
  if (attribute->content != NULL)
    freeString(attribute->content);
}

void freeXMLTree(XMLNode *node) {
  if (node->child != NULL) {
    // DEBUG_PRINT("Free child %s of %s\n", node->child->tag, node->tag);
    freeXMLTree(node->child);
  }

  if (node->sibling != NULL) {
    // DEBUG_PRINT("Free sibling %s of %s\n", node->sibling->tag, node->tag);
    freeXMLTree(node->sibling);
  }

  if (node->content != NULL) {
    // DEBUG_PRINT("Free content \"%s\" of %s\n", node->content, node->tag);
    freeString(node->content);
  }

  freeString(node->tag);
  freeString(node->closeTag);

  if (node->attributesSize > 0) {
    for (int a = 0; a < node->attributesSize; a++) {
      freeAttribute(node->attributes[a]);
      free(node->attributes[a]);
    }

    free(node->attributes);
  }

  free(node);
}
