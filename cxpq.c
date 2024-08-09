#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "xml-parser.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <xml_file>\n", argv[0]);
    return 0;
  }

  FILE *file = fopen(argv[1], "r");
  if (!file) {
    fprintf(stderr, "Failed to load file %s", argv[1]);
    return 1;
  }

  XMLDocument *document = parseXML(file);
  XMLElementNode *root = document->root;

  fclose(file);

  if (root == NULL) {
    freeXMLDocument(document);
    PRINT_ERROR("Failed to parse provided XML file\n");
    return 2;
  } else {
    printf("Root node %s\n", root->tag->value);

    printXMLTree((XMLNode *)root, 0);

    freeXMLDocument(document);
  }

  return 0;
}
