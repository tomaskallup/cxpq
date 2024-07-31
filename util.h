#include <stdbool.h>
#include <stdio.h>

#include "types.h"

#ifndef XML_UTIL_H

#define XML_UTIL_H

#define ERROR_MAX_LENGTH 50

#define PRINT_ERROR(...) fprintf(stderr, __VA_ARGS__)

bool isWhitespace(const char input);
bool isValidNameChar(const char input);
void skipWhitespaces(FILE *file);
void printCurrentLineMarked(FILE *file);

XMLNode *initNode();
Attribute *initAttribute();
void freeAttribute(Attribute *attribute);

void printXMLTree(XMLNode *root, int depth);
void freeXMLTree(XMLNode *root);
void freeString(String *str);

#endif
