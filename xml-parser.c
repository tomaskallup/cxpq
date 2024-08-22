#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node-collection.h"
#include "parsers/dtd.h"
#include "parsers/node.h"
#include "util.h"

void skipBOM(FILE *file) {
  unsigned char c[4];

  size_t nbytes = fread(c, sizeof(char), sizeof(c), file);

  int to_seek = -1;

  PRINT_DEBUG("BOM: ");

  if (nbytes >= 2) {
    if (memcmp(c, "\xFE\xFF", 2) == 0) {
      PRINT_DEBUG("UTF-16BE\n");
      to_seek = nbytes - 2;
    } else if (memcmp(c, "\xFF\xFE", 2) == 0) {
      PRINT_DEBUG("UTF-16LE\n");
      to_seek = nbytes - 2;
    } else if (memcmp(c, "\xFE\xFF", 2) == 0) {
      PRINT_DEBUG("UTF-16BE\n");
      to_seek = nbytes - 2;
    } else if (memcmp(c, "\xFF\xFE", 2) == 0) {
      PRINT_DEBUG("UTF-16LE\n");
      to_seek = nbytes - 2;
    } else if (nbytes >= 3) {
      if (memcmp(c, "\xEF\xBB\xBF", 3) == 0) {
        PRINT_DEBUG("UTF-8\n");
        to_seek = nbytes - 3;
      } else if (nbytes == 4) {
        if (memcmp(c, "\xFF\xFE\x00\x00", 4) == 0) {
          PRINT_DEBUG("UTF-32LE\n");
          to_seek = 0;
        } else if (memcmp(c, "\x00\x00\xFE\xFF", 4) == 0) {
          PRINT_DEBUG("UTF-32BE\n");
          to_seek = 0;
        }
      }
    }
  }

  if (to_seek == -1) {
    PRINT_DEBUG("No BOM detected\n");
    /* Probably not a BOM */
    rewind(file);
  } else if (to_seek > 0)
    fseek(file, -to_seek, SEEK_CUR);
}

XMLDocument *parseXML(FILE *file) {
  skipBOM(file);
  skipWhitespaces(file);

  XMLDocument *document = malloc(sizeof(XMLDocument));
  document->nodes = initNodeCollection();
  document->rootIndex = -1;
  XMLElementNode *root = NULL;

  XMLNode *currentNode = NULL;

  while (fgetc(file) == '<') {
    currentNode = parseNode(file, NULL);

    if (currentNode != NULL) {
      if (currentNode->type == ELEMENT) {
        XMLElementNode *elementNode = (XMLElementNode *)currentNode;
        if (root == NULL) {
          root = elementNode;
          document->rootIndex = document->nodes->size;
          printf("Found root element %i\n", document->rootIndex);
        } else {
          PRINT_ERROR("Found multiple root elements\n");
          freeXMLNode(currentNode);
        }
      }

      addNodeToCollection(document->nodes, currentNode);
    }
    if (currentNode == NULL)
      break;

    skipWhitespaces(file);
  }

  return document;
}
