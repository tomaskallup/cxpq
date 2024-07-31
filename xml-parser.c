#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "parsers/comment.h"
#include "parsers/node.h"
#include "parsers/prolog.h"

XMLNode *parseRootNode(FILE *file) {
  skipWhitespaces(file);
  char openingSymbol = fgetc(file);
  if (openingSymbol != '<') {
    printCurrentLineMarked(file);
    PRINT_ERROR("Missing start of element `<`, root element must be present\n");

    return NULL;
  } else {

    return parseNode(file);
  }
}

bool checkDTD(FILE *file) { 
  return true;
}

XMLNode *parseXML(FILE *file) {
  if (!parseProlog(file))
    return NULL;

  if (checkDTD(file)) {
    // parseDTD(file);
  }

  XMLNode *root = parseRootNode(file);

  return root;
}

void printXMLTree(XMLNode *root, int depth) {
  for (int i = 0; i < depth; i++)
    printf("  ");
  printf("%s", root->tag->value);

  for (int a = 0; a < root->attributesSize; a++) {
    struct Attribute *attribute = root->attributes[a];
    printf(" %s=%s", attribute->name, attribute->content->value);
  }

  if (root->content->length > 0)
    printf(" content: %s", root->content->value);

  printf("\n");

  if (root->child != NULL)
    printXMLTree(root->child, depth + 1);

  if (root->sibling != NULL)
    printXMLTree(root->sibling, depth);
}
