#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include "node-collection.h"
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

  while (ftell(file) < position - 1) {
    const char currentChar = fgetc(file);
    if (currentChar == '\n') {
      lineNumber++;
      column = 1;
    }

    column++;
  }

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
  node->parent = NULL;

  if (type == ELEMENT) {
    XMLElementNode *elementNode = (XMLElementNode *)node;
    elementNode->tag = stringCreateEmpty();
    elementNode->closeTag = stringCreateEmpty();
    elementNode->attributes = NULL;
    elementNode->attributesSize = 0;
    elementNode->children = initNodeCollection();
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
    dtdNode->name = stringCreateEmpty();
    dtdNode->content = NULL;
    dtdNode->systemID = NULL;
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

void freeAttribute(Attribute *attribute) {
  if (attribute->name != NULL)
    freeString(attribute->name);

  if (attribute->content != NULL)
    freeString(attribute->content);
}

void freeXMLNode(XMLNode *node) {
  if (node == NULL)
    return;

  if (node->type == ELEMENT) {
    XMLElementNode *elementNode = (XMLElementNode *)node;
    if (elementNode->children != NULL) {
      for (int i = 0; i < elementNode->children->size; i++)
        freeXMLNode(elementNode->children->nodes[i]);

      freeNodeCollection(elementNode->children);
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
    freeString(dtdNode->name);
    if (dtdNode->content != NULL)
      freeString(dtdNode->content);
    if (dtdNode->systemID != NULL)
    freeString(dtdNode->systemID);
  }

  free(node);
}

void freeXMLDocument(XMLDocument *document) {
  for (int i = 0; i < document->nodes->size; i++) {
    freeXMLNode(document->nodes->nodes[i]);
  }

  freeNodeCollection(document->nodes);

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

    for (int i = 0; i < elementNode->children->size; i++)
      printXMLTree(elementNode->children->nodes[i], depth + 1);

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
    printf("<!DOCTYPE %s", dtdNode->name->value);

    if (dtdNode->systemID != NULL)
      printf(" SYSTEM \"%s\"", dtdNode->systemID->value);
    else if (dtdNode->content != NULL)
      printf(" [%s]", dtdNode->content->value);

    printf(">\n");

    break;
  }
  case PROCESSING_INSTRUCTION: {
    XMLProcessingInstructionNode *processingInstructionNode =
        (XMLProcessingInstructionNode *)root;
    printf("<?%s", processingInstructionNode->tag->value);

    for (int a = 0; a < processingInstructionNode->attributesSize; a++) {
      struct Attribute *attribute = processingInstructionNode->attributes[a];
      printf(" %s=\"%s\"", attribute->name->value, attribute->content->value);
    }

    printf("?>\n");
    break;
  }
  }
}

void printXMLDocument(XMLDocument *document) {
  NodeCollection *nodes = document->nodes;
  for (int i = 0; i < nodes->size; i++) {
    printXMLTree(nodes->nodes[i], 0);
  }
}
