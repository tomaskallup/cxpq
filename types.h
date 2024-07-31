#include <stdbool.h>

#include "string.h"

#ifndef TYPES_H

#define TYPES_H

#define CHUNK_SIZE 1024

typedef struct Attribute {
  char name[CHUNK_SIZE];
  String *content;
  bool hasNamespace;
} Attribute;

typedef struct XMLNode {
  String *tag;
  String *closeTag;
  String *content;
  struct XMLNode *sibling;
  struct XMLNode *child;
  struct XMLNode *parent;
  struct Attribute **attributes;
  int attributesSize;
  bool hasNamespace;
} XMLNode;

typedef struct Comment {
  String *content;
} Comment;

#endif
