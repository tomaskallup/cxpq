#include <stdbool.h>

#include "string.h"

#ifndef TYPES_H

#define TYPES_H

enum XMLNodeType {
  ELEMENT,
  COMMENT,
  TEXT,
  PROCESSING_INSTRUCTION,
  DTD,
};

typedef struct Attribute {
  String *name;
  String *content;
  bool hasNamespace;
} Attribute;

typedef struct XMLNode {
  enum XMLNodeType type;
  struct XMLNode *parent;
  struct XMLNode *sibling;
} XMLNode;

typedef struct XMLElementNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;
  struct XMLNode *sibling;

  /* Specific attributes */
  String *tag;
  String *closeTag;
  struct XMLNode *child;
  struct Attribute **attributes;
  int attributesSize;
  bool hasNamespace;
} XMLElementNode;

typedef struct XMLCommentNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;
  struct XMLNode *sibling;

  /* Specific attributes */
  String *content;
} XMLCommentNode;

typedef struct XMLTextNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;
  struct XMLNode *sibling;

  /* Specific attributes */
  String *content;
  bool isCDATA;
} XMLTextNode;

typedef struct XMLProcessingInstructionNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;
  struct XMLNode *sibling;

  /* Specific attributes */
  String *tag;
  struct Attribute **attributes;
  int attributesSize;
} XMLProcessingInstructionNode;

typedef struct XMLDTDNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;
  struct XMLNode *sibling;

  /* Specific attributes */
  String *content;
} XMLDTDNode;

typedef struct XMLDocument {
  XMLElementNode *root;
  XMLNode **meta;
  int metaSize;
} XMLDocument;

#endif
