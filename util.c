#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include "util.h"

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

  /* PRINT_DEBUG("Start %lu end %lu column %lu rewind %lu lineLength %lu\n",
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

XMLNode *initNode(enum XMLNodeType type) {
  XMLNode *node = NULL;

  switch (type) {
  case ELEMENT:
    node = malloc(sizeof(XMLElementNode));
    break;
  case COMMENT:
    node = malloc(sizeof(XMLCommentNode));
    break;
  case TEXT:
    node = malloc(sizeof(XMLTextNode));
    break;
  case PROCESSING_INSTRUCTION:
    node = malloc(sizeof(XMLProcessingInstructionNode));
    break;
  case DTD:
    node = malloc(sizeof(XMLDTDNode));
    break;
  default:
    node = malloc(sizeof(XMLNode));
  }

  node->type = type;
  node->sibling = NULL;
  node->parent = NULL;

  if (type == ELEMENT) {
    XMLElementNode *elementNode = (XMLElementNode *)node;
    elementNode->tag = stringCreateEmpty();
    elementNode->closeTag = stringCreateEmpty();
    elementNode->attributes = NULL;
    elementNode->attributesSize = 0;
    elementNode->child = NULL;
  } else if (type == COMMENT) {
    XMLCommentNode *commentNode = (XMLCommentNode *)node;
    commentNode->content = stringCreateEmpty();
  } else if (type == TEXT) {
    XMLTextNode *textNode = (XMLTextNode *)node;
    textNode->content = stringCreateEmpty();
    textNode->isCDATA = false;
  } else if (type == PROCESSING_INSTRUCTION) {
    XMLProcessingInstructionNode *processingInstructionNode =
        (XMLProcessingInstructionNode *)node;
    processingInstructionNode->tag = stringCreateEmpty();
    processingInstructionNode->attributes = NULL;
    processingInstructionNode->attributesSize = 0;
  } else if (type == DTD) {
    XMLDTDNode *dtdNode = (XMLDTDNode *)node;
    dtdNode->content = stringCreateEmpty();
  }

  return node;
}

Attribute *initAttribute() {
  Attribute *attribute = malloc(sizeof(Attribute));

  attribute->name = stringCreateEmpty();
  attribute->content = stringCreateEmpty();
  attribute->hasNamespace = false;

  return attribute;
}

void freeString(String *str) {
  if (str->value != NULL) {
    free(str->value);
  }

  free(str);
}

void freeAttribute(Attribute *attribute) {
  if (attribute->name != NULL)
    freeString(attribute->name);

  if (attribute->content != NULL)
    freeString(attribute->content);
}

void freeXMLTree(XMLNode *node) {
  if (node == NULL)
    return;

  if (node->type == ELEMENT) {
    XMLElementNode *elementNode = (XMLElementNode *)node;
    if (elementNode->child != NULL) {
      freeXMLTree(elementNode->child);
    }

    freeString(elementNode->tag);
    freeString(elementNode->closeTag);

    if (elementNode->attributesSize > 0) {
      for (int a = 0; a < elementNode->attributesSize; a++) {
        freeAttribute(elementNode->attributes[a]);
        free(elementNode->attributes[a]);
      }

      free(elementNode->attributes);
    }
  } else if (node->type == COMMENT) {
    XMLCommentNode *commentNode = (XMLCommentNode *)node;
    freeString(commentNode->content);
  } else if (node->type == TEXT) {
    XMLTextNode *textNode = (XMLTextNode *)node;
    freeString(textNode->content);
  } else if (node->type == PROCESSING_INSTRUCTION) {
    XMLProcessingInstructionNode *processingInstructionNode =
        (XMLProcessingInstructionNode *)node;
    freeString(processingInstructionNode->tag);
    if (processingInstructionNode->attributesSize > 0) {
      for (int a = 0; a < processingInstructionNode->attributesSize; a++) {
        freeAttribute(processingInstructionNode->attributes[a]);
        free(processingInstructionNode->attributes[a]);
      }

      free(processingInstructionNode->attributes);
    }
  } else if (node->type == DTD) {
    XMLDTDNode *dtdNode = (XMLDTDNode *)node;
    freeString(dtdNode->content);
  }

  if (node->sibling != NULL) {
    freeXMLTree(node->sibling);
  }

  free(node);
}

void freeXMLDocument(XMLDocument *document) {
  if (document->root != NULL)
    freeXMLTree((XMLNode *)document->root);

  if (document->metaSize > 0) {
    for (int i = 0; i < document->metaSize; i++) {
      freeXMLTree(document->meta[i]);
    }
  }

  free(document->meta);

  free(document);
}

void printXMLTree(XMLNode *root, int depth) {
  for (int i = 0; i < depth; i++)
    printf("  ");

  switch (root->type) {
  case ELEMENT: {
    XMLElementNode *elementNode = (XMLElementNode *)root;
    printf("<%s", elementNode->tag->value);

    for (int a = 0; a < elementNode->attributesSize; a++) {
      struct Attribute *attribute = elementNode->attributes[a];
      printf(" %s=\"%s\"", attribute->name->value, attribute->content->value);
    }

    printf(">\n");

    if (elementNode->child != NULL)
      printXMLTree(elementNode->child, depth + 1);

    for (int i = 0; i < depth; i++)
      printf("  ");
    printf("</%s>\n", elementNode->tag->value);
    break;
  }
  case COMMENT: {
    XMLCommentNode *commentNode = (XMLCommentNode *)root;
    printf("<!--%s-->\n", commentNode->content->value);

    break;
  }
  case TEXT: {
    XMLTextNode *textNode = (XMLTextNode *)root;
    if (textNode->isCDATA)
      printf("<![CDATA[");
    printf("%s", textNode->content->value);
    if (textNode->isCDATA)
      printf("]]>");

    printf("\n");

    break;
  }
  case DTD: {
    XMLDTDNode *dtdNode = (XMLDTDNode *)root;
    printf("<!DOCTYPE %s>\n", dtdNode->content->value);

    break;
  }
  case PROCESSING_INSTRUCTION: {
    XMLProcessingInstructionNode *processingInstructionNode =
        (XMLProcessingInstructionNode *)root;
    printf("<?%s", processingInstructionNode->tag->value);

    for (int a = 0; a < processingInstructionNode->attributesSize; a++) {
      struct Attribute *attribute = processingInstructionNode->attributes[a];
      printf(" %s=%s", attribute->name->value, attribute->content->value);
    }

    printf("?>\n");
    break;
  }
  }

  if (root->sibling != NULL)
    printXMLTree(root->sibling, depth);
}
