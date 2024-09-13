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
} XMLNode;

typedef struct NodeCollection {
  XMLNode **nodes;
  unsigned int size;
  unsigned int allocated;
  unsigned int lastIndex;
} NodeCollection;

typedef struct XMLElementNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;

  /* Specific attributes */
  String *tag;
  String *closeTag;
  NodeCollection *children;
  struct Attribute **attributes;
  unsigned int attributesSize;
  bool hasNamespace;
} XMLElementNode;

typedef struct XMLCommentNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;

  /* Specific attributes */
  String *content;
} XMLCommentNode;

typedef struct XMLTextNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;

  /* Specific attributes */
  String *content;
  bool isCDATA;
} XMLTextNode;

typedef struct XMLProcessingInstructionNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;

  /* Specific attributes */
  String *tag;
  struct Attribute **attributes;
  unsigned int attributesSize;
} XMLProcessingInstructionNode;

typedef struct XMLDTDNode {
  /* Generic attributes */
  enum XMLNodeType type;
  struct XMLNode *parent;

  /* Specific attributes */
  String *content;
  String *name;
  String *systemID;
} XMLDTDNode;

typedef struct XMLDocument {
  NodeCollection *nodes;
  unsigned int rootIndex;
} XMLDocument;

#endif
