#include <stdbool.h>
#include <stdio.h>

#include "types.h"

#ifndef XML_UTIL_H

#define XML_UTIL_H

#define ERROR_MAX_LENGTH 50

#define PRINT_ERROR(...) fprintf(stderr, __VA_ARGS__)

#ifdef DEBUG
#define PRINT_DEBUG(...) printf(__VA_ARGS__)
#else
#define PRINT_DEBUG(...)
#endif


bool isWhitespace(const char input);
bool isValidNameChar(const char input);
void skipWhitespaces(FILE *file);
void printCurrentLineMarked(FILE *file);

XMLNode *initNode(enum XMLNodeType type);
Attribute *initAttribute(void);
void freeAttribute(Attribute *attribute);

void freeXMLNode(XMLNode *root);
void freeXMLDocument(XMLDocument *document);
void printXMLTree(XMLNode *root, int depth);
void printXMLDocument(XMLDocument *document);

#endif
