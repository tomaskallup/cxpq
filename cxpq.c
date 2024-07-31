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

  XMLNode *root = parseXML(file);

  fclose(file);

  if (root == NULL) {
    PRINT_ERROR("Failed to parse provided XML file\n");
    return 2;
  } else {
    printf("Root node %s, content: %s\n", root->tag->value,
           root->content->value);

    printXMLTree(root, 0);

    freeXMLTree(root);
  }

  return 0;
}
